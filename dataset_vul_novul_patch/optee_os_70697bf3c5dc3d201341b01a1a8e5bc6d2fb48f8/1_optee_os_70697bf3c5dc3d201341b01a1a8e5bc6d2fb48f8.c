TEE_Result syscall_asymm_operate(unsigned long state,
			const struct utee_attribute *usr_params,
			size_t num_params, const void *src_data, size_t src_len,
			void *dst_data, uint64_t *dst_len)
{
	TEE_Result res;
	struct tee_cryp_state *cs;
	struct tee_ta_session *sess;
	uint64_t dlen64;
	size_t dlen;
	struct tee_obj *o;
	void *label = NULL;
	size_t label_len = 0;
	size_t n;
	int salt_len;
	TEE_Attribute *params = NULL;
	struct user_ta_ctx *utc;

	res = tee_ta_get_current_session(&sess);
	if (res != TEE_SUCCESS)
		return res;
	utc = to_user_ta_ctx(sess->ctx);

	res = tee_svc_cryp_get_state(sess, tee_svc_uref_to_vaddr(state), &cs);
	if (res != TEE_SUCCESS)
		return res;

	res = tee_mmu_check_access_rights(
		utc,
		TEE_MEMORY_ACCESS_READ | TEE_MEMORY_ACCESS_ANY_OWNER,
		(uaddr_t) src_data, src_len);
	if (res != TEE_SUCCESS)
		return res;

	res = tee_svc_copy_from_user(&dlen64, dst_len, sizeof(dlen64));
	if (res != TEE_SUCCESS)
		return res;
	dlen = dlen64;

	res = tee_mmu_check_access_rights(
		utc,
		TEE_MEMORY_ACCESS_READ | TEE_MEMORY_ACCESS_WRITE |
			TEE_MEMORY_ACCESS_ANY_OWNER,
		(uaddr_t) dst_data, dlen);
 	if (res != TEE_SUCCESS)
 		return res;
 
	params = malloc(sizeof(TEE_Attribute) * num_params);
 	if (!params)
 		return TEE_ERROR_OUT_OF_MEMORY;
 	res = copy_in_attrs(utc, usr_params, num_params, params);
	if (res != TEE_SUCCESS)
		goto out;

	res = tee_obj_get(utc, cs->key1, &o);
	if (res != TEE_SUCCESS)
		goto out;
	if ((o->info.handleFlags & TEE_HANDLE_FLAG_INITIALIZED) == 0) {
		res = TEE_ERROR_GENERIC;
		goto out;
	}

	switch (cs->algo) {
	case TEE_ALG_RSA_NOPAD:
		if (cs->mode == TEE_MODE_ENCRYPT) {
			res = crypto_acipher_rsanopad_encrypt(o->attr, src_data,
							      src_len, dst_data,
							      &dlen);
		} else if (cs->mode == TEE_MODE_DECRYPT) {
			res = crypto_acipher_rsanopad_decrypt(o->attr, src_data,
							      src_len, dst_data,
							      &dlen);
		} else {
			/*
			 * We will panic because "the mode is not compatible
			 * with the function"
			 */
			res = TEE_ERROR_GENERIC;
		}
		break;

	case TEE_ALG_RSAES_PKCS1_V1_5:
	case TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1:
	case TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA224:
	case TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA256:
	case TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA384:
	case TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA512:
		for (n = 0; n < num_params; n++) {
			if (params[n].attributeID == TEE_ATTR_RSA_OAEP_LABEL) {
				label = params[n].content.ref.buffer;
				label_len = params[n].content.ref.length;
				break;
			}
		}

		if (cs->mode == TEE_MODE_ENCRYPT) {
			res = crypto_acipher_rsaes_encrypt(cs->algo, o->attr,
							   label, label_len,
							   src_data, src_len,
							   dst_data, &dlen);
		} else if (cs->mode == TEE_MODE_DECRYPT) {
			res = crypto_acipher_rsaes_decrypt(
					cs->algo, o->attr, label, label_len,
					src_data, src_len, dst_data, &dlen);
		} else {
			res = TEE_ERROR_BAD_PARAMETERS;
		}
		break;

#if defined(CFG_CRYPTO_RSASSA_NA1)
	case TEE_ALG_RSASSA_PKCS1_V1_5:
#endif
	case TEE_ALG_RSASSA_PKCS1_V1_5_MD5:
	case TEE_ALG_RSASSA_PKCS1_V1_5_SHA1:
	case TEE_ALG_RSASSA_PKCS1_V1_5_SHA224:
	case TEE_ALG_RSASSA_PKCS1_V1_5_SHA256:
	case TEE_ALG_RSASSA_PKCS1_V1_5_SHA384:
	case TEE_ALG_RSASSA_PKCS1_V1_5_SHA512:
	case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA1:
	case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA224:
	case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256:
	case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA384:
	case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA512:
		if (cs->mode != TEE_MODE_SIGN) {
			res = TEE_ERROR_BAD_PARAMETERS;
			break;
		}
		salt_len = pkcs1_get_salt_len(params, num_params, src_len);
		res = crypto_acipher_rsassa_sign(cs->algo, o->attr, salt_len,
						 src_data, src_len, dst_data,
						 &dlen);
		break;

	case TEE_ALG_DSA_SHA1:
	case TEE_ALG_DSA_SHA224:
	case TEE_ALG_DSA_SHA256:
		res = crypto_acipher_dsa_sign(cs->algo, o->attr, src_data,
					      src_len, dst_data, &dlen);
		break;
	case TEE_ALG_ECDSA_P192:
	case TEE_ALG_ECDSA_P224:
	case TEE_ALG_ECDSA_P256:
	case TEE_ALG_ECDSA_P384:
	case TEE_ALG_ECDSA_P521:
		res = crypto_acipher_ecc_sign(cs->algo, o->attr, src_data,
					      src_len, dst_data, &dlen);
		break;

	default:
		res = TEE_ERROR_BAD_PARAMETERS;
		break;
	}

out:
	free(params);

	if (res == TEE_SUCCESS || res == TEE_ERROR_SHORT_BUFFER) {
		TEE_Result res2;

		dlen64 = dlen;
		res2 = tee_svc_copy_to_user(dst_len, &dlen64, sizeof(*dst_len));
		if (res2 != TEE_SUCCESS)
			return res2;
	}

	return res;
}
