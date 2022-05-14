TEE_Result syscall_asymm_verify(unsigned long state,
			const struct utee_attribute *usr_params,
			size_t num_params, const void *data, size_t data_len,
			const void *sig, size_t sig_len)
{
	TEE_Result res;
	struct tee_cryp_state *cs;
	struct tee_ta_session *sess;
	struct tee_obj *o;
	size_t hash_size;
	int salt_len = 0;
	TEE_Attribute *params = NULL;
	uint32_t hash_algo;
	struct user_ta_ctx *utc;

	res = tee_ta_get_current_session(&sess);
	if (res != TEE_SUCCESS)
		return res;
	utc = to_user_ta_ctx(sess->ctx);

	res = tee_svc_cryp_get_state(sess, tee_svc_uref_to_vaddr(state), &cs);
	if (res != TEE_SUCCESS)
		return res;

	if (cs->mode != TEE_MODE_VERIFY)
		return TEE_ERROR_BAD_PARAMETERS;

	res = tee_mmu_check_access_rights(utc,
					  TEE_MEMORY_ACCESS_READ |
					  TEE_MEMORY_ACCESS_ANY_OWNER,
					  (uaddr_t)data, data_len);
	if (res != TEE_SUCCESS)
		return res;

	res = tee_mmu_check_access_rights(utc,
					  TEE_MEMORY_ACCESS_READ |
					  TEE_MEMORY_ACCESS_ANY_OWNER,
					  (uaddr_t)sig, sig_len);
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
		res = TEE_ERROR_BAD_PARAMETERS;
		goto out;
	}

	switch (TEE_ALG_GET_MAIN_ALG(cs->algo)) {
	case TEE_MAIN_ALGO_RSA:
		if (cs->algo != TEE_ALG_RSASSA_PKCS1_V1_5) {
			hash_algo = TEE_DIGEST_HASH_TO_ALGO(cs->algo);
			res = tee_hash_get_digest_size(hash_algo, &hash_size);
			if (res != TEE_SUCCESS)
				break;
			if (data_len != hash_size) {
				res = TEE_ERROR_BAD_PARAMETERS;
				break;
			}
			salt_len = pkcs1_get_salt_len(params, num_params,
						      hash_size);
		}
		res = crypto_acipher_rsassa_verify(cs->algo, o->attr, salt_len,
						   data, data_len, sig,
						   sig_len);
		break;

	case TEE_MAIN_ALGO_DSA:
		hash_algo = TEE_DIGEST_HASH_TO_ALGO(cs->algo);
		res = tee_hash_get_digest_size(hash_algo, &hash_size);
		if (res != TEE_SUCCESS)
			break;
		/*
		 * Depending on the DSA algorithm (NIST), the digital signature
		 * output size may be truncated to the size of a key pair
		 * (Q prime size). Q prime size must be less or equal than the
		 * hash output length of the hash algorithm involved.
		 */
		if (data_len > hash_size) {
			res = TEE_ERROR_BAD_PARAMETERS;
			break;
		}
		res = crypto_acipher_dsa_verify(cs->algo, o->attr, data,
						data_len, sig, sig_len);
		break;

	case TEE_MAIN_ALGO_ECDSA:
		res = crypto_acipher_ecc_verify(cs->algo, o->attr, data,
						data_len, sig, sig_len);
		break;

	default:
		res = TEE_ERROR_NOT_SUPPORTED;
	}

out:
	free(params);
	return res;
}
