TEE_Result syscall_cryp_derive_key(unsigned long state,
			const struct utee_attribute *usr_params,
			unsigned long param_count, unsigned long derived_key)
{
	TEE_Result res = TEE_ERROR_NOT_SUPPORTED;
	struct tee_ta_session *sess;
	struct tee_obj *ko;
	struct tee_obj *so;
	struct tee_cryp_state *cs;
	struct tee_cryp_obj_secret *sk;
	const struct tee_cryp_obj_type_props *type_props;
	TEE_Attribute *params = NULL;
	struct user_ta_ctx *utc;

	res = tee_ta_get_current_session(&sess);
	if (res != TEE_SUCCESS)
		return res;
	utc = to_user_ta_ctx(sess->ctx);

	res = tee_svc_cryp_get_state(sess, tee_svc_uref_to_vaddr(state), &cs);
 	if (res != TEE_SUCCESS)
 		return res;
 
	params = malloc(sizeof(TEE_Attribute) * param_count);
 	if (!params)
 		return TEE_ERROR_OUT_OF_MEMORY;
 	res = copy_in_attrs(utc, usr_params, param_count, params);
	if (res != TEE_SUCCESS)
		goto out;

	/* Get key set in operation */
	res = tee_obj_get(utc, cs->key1, &ko);
	if (res != TEE_SUCCESS)
		goto out;

	res = tee_obj_get(utc, tee_svc_uref_to_vaddr(derived_key), &so);
	if (res != TEE_SUCCESS)
		goto out;

	/* Find information needed about the object to initialize */
	sk = so->attr;

	/* Find description of object */
	type_props = tee_svc_find_type_props(so->info.objectType);
	if (!type_props) {
		res = TEE_ERROR_NOT_SUPPORTED;
		goto out;
	}

	if (cs->algo == TEE_ALG_DH_DERIVE_SHARED_SECRET) {
		size_t alloc_size;
		struct bignum *pub;
		struct bignum *ss;

		if (param_count != 1 ||
		    params[0].attributeID != TEE_ATTR_DH_PUBLIC_VALUE) {
			res = TEE_ERROR_BAD_PARAMETERS;
			goto out;
		}

		alloc_size = params[0].content.ref.length * 8;
		pub = crypto_bignum_allocate(alloc_size);
		ss = crypto_bignum_allocate(alloc_size);
		if (pub && ss) {
			crypto_bignum_bin2bn(params[0].content.ref.buffer,
					     params[0].content.ref.length, pub);
			res = crypto_acipher_dh_shared_secret(ko->attr,
							      pub, ss);
			if (res == TEE_SUCCESS) {
				sk->key_size = crypto_bignum_num_bytes(ss);
				crypto_bignum_bn2bin(ss, (uint8_t *)(sk + 1));
				so->info.handleFlags |=
						TEE_HANDLE_FLAG_INITIALIZED;
				set_attribute(so, type_props,
					      TEE_ATTR_SECRET_VALUE);
			}
		} else {
			res = TEE_ERROR_OUT_OF_MEMORY;
		}
		crypto_bignum_free(pub);
		crypto_bignum_free(ss);
	} else if (TEE_ALG_GET_MAIN_ALG(cs->algo) == TEE_MAIN_ALGO_ECDH) {
		size_t alloc_size;
		struct ecc_public_key key_public;
		uint8_t *pt_secret;
		unsigned long pt_secret_len;

		if (param_count != 2 ||
		    params[0].attributeID != TEE_ATTR_ECC_PUBLIC_VALUE_X ||
		    params[1].attributeID != TEE_ATTR_ECC_PUBLIC_VALUE_Y) {
			res = TEE_ERROR_BAD_PARAMETERS;
			goto out;
		}

		switch (cs->algo) {
		case TEE_ALG_ECDH_P192:
			alloc_size = 192;
			break;
		case TEE_ALG_ECDH_P224:
			alloc_size = 224;
			break;
		case TEE_ALG_ECDH_P256:
			alloc_size = 256;
			break;
		case TEE_ALG_ECDH_P384:
			alloc_size = 384;
			break;
		case TEE_ALG_ECDH_P521:
			alloc_size = 521;
			break;
		default:
			res = TEE_ERROR_NOT_IMPLEMENTED;
			goto out;
		}

		/* Create the public key */
		res = crypto_acipher_alloc_ecc_public_key(&key_public,
							  alloc_size);
		if (res != TEE_SUCCESS)
			goto out;
		key_public.curve = ((struct ecc_keypair *)ko->attr)->curve;
		crypto_bignum_bin2bn(params[0].content.ref.buffer,
				     params[0].content.ref.length,
				     key_public.x);
		crypto_bignum_bin2bn(params[1].content.ref.buffer,
				     params[1].content.ref.length,
				     key_public.y);

		pt_secret = (uint8_t *)(sk + 1);
		pt_secret_len = sk->alloc_size;
		res = crypto_acipher_ecc_shared_secret(ko->attr, &key_public,
						       pt_secret,
						       &pt_secret_len);

		if (res == TEE_SUCCESS) {
			sk->key_size = pt_secret_len;
			so->info.handleFlags |= TEE_HANDLE_FLAG_INITIALIZED;
			set_attribute(so, type_props, TEE_ATTR_SECRET_VALUE);
		}

		/* free the public key */
		crypto_acipher_free_ecc_public_key(&key_public);
	}
#if defined(CFG_CRYPTO_HKDF)
	else if (TEE_ALG_GET_MAIN_ALG(cs->algo) == TEE_MAIN_ALGO_HKDF) {
		void *salt, *info;
		size_t salt_len, info_len, okm_len;
		uint32_t hash_id = TEE_ALG_GET_DIGEST_HASH(cs->algo);
		struct tee_cryp_obj_secret *ik = ko->attr;
		const uint8_t *ikm = (const uint8_t *)(ik + 1);

		res = get_hkdf_params(params, param_count, &salt, &salt_len,
				      &info, &info_len, &okm_len);
		if (res != TEE_SUCCESS)
			goto out;

		/* Requested size must fit into the output object's buffer */
		if (okm_len > ik->alloc_size) {
			res = TEE_ERROR_BAD_PARAMETERS;
			goto out;
		}

		res = tee_cryp_hkdf(hash_id, ikm, ik->key_size, salt, salt_len,
				    info, info_len, (uint8_t *)(sk + 1),
				    okm_len);
		if (res == TEE_SUCCESS) {
			sk->key_size = okm_len;
			so->info.handleFlags |= TEE_HANDLE_FLAG_INITIALIZED;
			set_attribute(so, type_props, TEE_ATTR_SECRET_VALUE);
		}
	}
#endif
#if defined(CFG_CRYPTO_CONCAT_KDF)
	else if (TEE_ALG_GET_MAIN_ALG(cs->algo) == TEE_MAIN_ALGO_CONCAT_KDF) {
		void *info;
		size_t info_len, derived_key_len;
		uint32_t hash_id = TEE_ALG_GET_DIGEST_HASH(cs->algo);
		struct tee_cryp_obj_secret *ss = ko->attr;
		const uint8_t *shared_secret = (const uint8_t *)(ss + 1);

		res = get_concat_kdf_params(params, param_count, &info,
					    &info_len, &derived_key_len);
		if (res != TEE_SUCCESS)
			goto out;

		/* Requested size must fit into the output object's buffer */
		if (derived_key_len > ss->alloc_size) {
			res = TEE_ERROR_BAD_PARAMETERS;
			goto out;
		}

		res = tee_cryp_concat_kdf(hash_id, shared_secret, ss->key_size,
					  info, info_len, (uint8_t *)(sk + 1),
					  derived_key_len);
		if (res == TEE_SUCCESS) {
			sk->key_size = derived_key_len;
			so->info.handleFlags |= TEE_HANDLE_FLAG_INITIALIZED;
			set_attribute(so, type_props, TEE_ATTR_SECRET_VALUE);
		}
	}
#endif
#if defined(CFG_CRYPTO_PBKDF2)
	else if (TEE_ALG_GET_MAIN_ALG(cs->algo) == TEE_MAIN_ALGO_PBKDF2) {
		void *salt;
		size_t salt_len, iteration_count, derived_key_len;
		uint32_t hash_id = TEE_ALG_GET_DIGEST_HASH(cs->algo);
		struct tee_cryp_obj_secret *ss = ko->attr;
		const uint8_t *password = (const uint8_t *)(ss + 1);

		res = get_pbkdf2_params(params, param_count, &salt, &salt_len,
					&derived_key_len, &iteration_count);
		if (res != TEE_SUCCESS)
			goto out;

		/* Requested size must fit into the output object's buffer */
		if (derived_key_len > ss->alloc_size) {
			res = TEE_ERROR_BAD_PARAMETERS;
			goto out;
		}

		res = tee_cryp_pbkdf2(hash_id, password, ss->key_size, salt,
				      salt_len, iteration_count,
				      (uint8_t *)(sk + 1), derived_key_len);
		if (res == TEE_SUCCESS) {
			sk->key_size = derived_key_len;
			so->info.handleFlags |= TEE_HANDLE_FLAG_INITIALIZED;
			set_attribute(so, type_props, TEE_ATTR_SECRET_VALUE);
		}
	}
#endif
	else
		res = TEE_ERROR_NOT_SUPPORTED;

out:
	free(params);
	return res;
}
