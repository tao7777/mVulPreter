auth_read_binary(struct sc_card *card, unsigned int offset,
		unsigned char *buf, size_t count, unsigned long flags)
{
	int rv;
	struct sc_pkcs15_bignum bn[2];
	unsigned char *out = NULL;
	bn[0].data = NULL;
 	bn[1].data = NULL;
 
 	LOG_FUNC_CALLED(card->ctx);
 	sc_log(card->ctx,
 	       "offset %i; size %"SC_FORMAT_LEN_SIZE_T"u; flags 0x%lX",
 	       offset, count, flags);
	sc_log(card->ctx,"last selected : magic %X; ef %X",
			auth_current_ef->magic, auth_current_ef->ef_structure);

	if (offset & ~0x7FFF)
		LOG_TEST_RET(card->ctx, SC_ERROR_INVALID_ARGUMENTS, "Invalid file offset");

	if (auth_current_ef->magic==SC_FILE_MAGIC &&
			auth_current_ef->ef_structure == SC_CARDCTL_OBERTHUR_KEY_RSA_PUBLIC)   {
		int jj;
		unsigned char resp[256];
		size_t resp_len, out_len;
		struct sc_pkcs15_pubkey_rsa key;

		resp_len = sizeof(resp);
		rv = auth_read_component(card, SC_CARDCTL_OBERTHUR_KEY_RSA_PUBLIC,
				2, resp, resp_len);
		LOG_TEST_RET(card->ctx, rv, "read component failed");

		for (jj=0; jj<rv && *(resp+jj)==0; jj++)
			;

		bn[0].data = calloc(1, rv - jj);
		if (!bn[0].data) {
			rv = SC_ERROR_OUT_OF_MEMORY;
			goto err;
		}
		bn[0].len = rv - jj;
		memcpy(bn[0].data, resp + jj, rv - jj);

		rv = auth_read_component(card, SC_CARDCTL_OBERTHUR_KEY_RSA_PUBLIC,
				1, resp, resp_len);
		LOG_TEST_GOTO_ERR(card->ctx, rv, "Cannot read RSA public key component");

		bn[1].data = calloc(1, rv);
		if (!bn[1].data) {
			rv = SC_ERROR_OUT_OF_MEMORY;
			goto err;
		}
		bn[1].len = rv;
		memcpy(bn[1].data, resp, rv);

		key.exponent = bn[0];
		key.modulus = bn[1];

		if (sc_pkcs15_encode_pubkey_rsa(card->ctx, &key, &out, &out_len)) {
			rv = SC_ERROR_INVALID_ASN1_OBJECT;
			LOG_TEST_GOTO_ERR(card->ctx, rv, "cannot encode RSA public key");
		}
		else {
			rv  = out_len - offset > count ? count : out_len - offset;
			memcpy(buf, out + offset, rv);

			sc_log_hex(card->ctx, "write_publickey", buf, rv);
		}
	}
	else {
		rv = iso_ops->read_binary(card, offset, buf, count, 0);
	}

err:
	free(bn[0].data);
	free(bn[1].data);
	free(out);

	LOG_FUNC_RETURN(card->ctx, rv);
}
