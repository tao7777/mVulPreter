TEE_Result syscall_obj_generate_key(unsigned long obj, unsigned long key_size,
			const struct utee_attribute *usr_params,
			unsigned long param_count)
{
	TEE_Result res;
	struct tee_ta_session *sess;
	const struct tee_cryp_obj_type_props *type_props;
	struct tee_obj *o;
	struct tee_cryp_obj_secret *key;
	size_t byte_size;
	TEE_Attribute *params = NULL;

	res = tee_ta_get_current_session(&sess);
	if (res != TEE_SUCCESS)
		return res;

	res = tee_obj_get(to_user_ta_ctx(sess->ctx),
			  tee_svc_uref_to_vaddr(obj), &o);
	if (res != TEE_SUCCESS)
		return res;

	/* Must be a transient object */
	if ((o->info.handleFlags & TEE_HANDLE_FLAG_PERSISTENT) != 0)
		return TEE_ERROR_BAD_STATE;

	/* Must not be initialized already */
	if ((o->info.handleFlags & TEE_HANDLE_FLAG_INITIALIZED) != 0)
		return TEE_ERROR_BAD_STATE;

	/* Find description of object */
	type_props = tee_svc_find_type_props(o->info.objectType);
	if (!type_props)
		return TEE_ERROR_NOT_SUPPORTED;

	/* Check that maxKeySize follows restrictions */
	if (key_size % type_props->quanta != 0)
		return TEE_ERROR_NOT_SUPPORTED;
	if (key_size < type_props->min_size)
		return TEE_ERROR_NOT_SUPPORTED;
 	if (key_size > type_props->max_size)
 		return TEE_ERROR_NOT_SUPPORTED;
 
	params = malloc(sizeof(TEE_Attribute) * param_count);
 	if (!params)
 		return TEE_ERROR_OUT_OF_MEMORY;
 	res = copy_in_attrs(to_user_ta_ctx(sess->ctx), usr_params, param_count,
			    params);
	if (res != TEE_SUCCESS)
		goto out;

	res = tee_svc_cryp_check_attr(ATTR_USAGE_GENERATE_KEY, type_props,
				      params, param_count);
	if (res != TEE_SUCCESS)
		goto out;

	switch (o->info.objectType) {
	case TEE_TYPE_AES:
	case TEE_TYPE_DES:
	case TEE_TYPE_DES3:
	case TEE_TYPE_HMAC_MD5:
	case TEE_TYPE_HMAC_SHA1:
	case TEE_TYPE_HMAC_SHA224:
	case TEE_TYPE_HMAC_SHA256:
	case TEE_TYPE_HMAC_SHA384:
	case TEE_TYPE_HMAC_SHA512:
	case TEE_TYPE_GENERIC_SECRET:
		byte_size = key_size / 8;

		/*
		 * We have to do it like this because the parity bits aren't
		 * counted when telling the size of the key in bits.
		 */
		if (o->info.objectType == TEE_TYPE_DES ||
		    o->info.objectType == TEE_TYPE_DES3) {
			byte_size = (key_size + key_size / 7) / 8;
		}

		key = (struct tee_cryp_obj_secret *)o->attr;
		if (byte_size > key->alloc_size) {
			res = TEE_ERROR_EXCESS_DATA;
			goto out;
		}

		res = crypto_rng_read((void *)(key + 1), byte_size);
		if (res != TEE_SUCCESS)
			goto out;

		key->key_size = byte_size;

		/* Set bits for all known attributes for this object type */
		o->have_attrs = (1 << type_props->num_type_attrs) - 1;

		break;

	case TEE_TYPE_RSA_KEYPAIR:
		res = tee_svc_obj_generate_key_rsa(o, type_props, key_size,
						   params, param_count);
		if (res != TEE_SUCCESS)
			goto out;
		break;

	case TEE_TYPE_DSA_KEYPAIR:
		res = tee_svc_obj_generate_key_dsa(o, type_props, key_size);
		if (res != TEE_SUCCESS)
			goto out;
		break;

	case TEE_TYPE_DH_KEYPAIR:
		res = tee_svc_obj_generate_key_dh(o, type_props, key_size,
						  params, param_count);
		if (res != TEE_SUCCESS)
			goto out;
		break;

	case TEE_TYPE_ECDSA_KEYPAIR:
	case TEE_TYPE_ECDH_KEYPAIR:
		res = tee_svc_obj_generate_key_ecc(o, type_props, key_size,
						  params, param_count);
		if (res != TEE_SUCCESS)
			goto out;
		break;

	default:
		res = TEE_ERROR_BAD_FORMAT;
	}

out:
	free(params);
	if (res == TEE_SUCCESS) {
		o->info.keySize = key_size;
		o->info.handleFlags |= TEE_HANDLE_FLAG_INITIALIZED;
	}
	return res;
}
