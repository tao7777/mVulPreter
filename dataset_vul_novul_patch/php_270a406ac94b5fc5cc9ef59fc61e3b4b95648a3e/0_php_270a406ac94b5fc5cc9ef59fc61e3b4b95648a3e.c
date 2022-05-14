PHP_FUNCTION(openssl_encrypt)
{
	zend_bool raw_output = 0;
	char *data, *method, *password, *iv = "";
        int data_len, method_len, password_len, iv_len = 0, max_iv_len;
        const EVP_CIPHER *cipher_type;
        EVP_CIPHER_CTX cipher_ctx;
       int i = 0, outlen, keylen;
        unsigned char *outbuf, *key;
        zend_bool free_iv;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|bs", &data, &data_len, &method, &method_len, &password, &password_len, &raw_output, &iv, &iv_len) == FAILURE) {
		return;
	}
	cipher_type = EVP_get_cipherbyname(method);
	if (!cipher_type) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	keylen = EVP_CIPHER_key_length(cipher_type);
	if (keylen > password_len) {
		key = emalloc(keylen);
		memset(key, 0, keylen);
		memcpy(key, password, password_len);
	} else {
		key = (unsigned char*)password;
	}

	max_iv_len = EVP_CIPHER_iv_length(cipher_type);
	if (iv_len <= 0 && max_iv_len > 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Using an empty Initialization Vector (iv) is potentially insecure and not recommended");
	}
	free_iv = php_openssl_validate_iv(&iv, &iv_len, max_iv_len TSRMLS_CC);

	outlen = data_len + EVP_CIPHER_block_size(cipher_type);
	outbuf = emalloc(outlen + 1);

	EVP_EncryptInit(&cipher_ctx, cipher_type, NULL, NULL);
	if (password_len > keylen) {
		EVP_CIPHER_CTX_set_key_length(&cipher_ctx, password_len);
	}
	EVP_EncryptInit_ex(&cipher_ctx, NULL, NULL, key, (unsigned char *)iv);
	if (data_len > 0) {
		EVP_EncryptUpdate(&cipher_ctx, outbuf, &i, (unsigned char *)data, data_len);
	}
	outlen = i;
	if (EVP_EncryptFinal(&cipher_ctx, (unsigned char *)outbuf + i, &i)) {
		outlen += i;
		if (raw_output) {
			outbuf[outlen] = '\0';
			RETVAL_STRINGL((char *)outbuf, outlen, 0);
		} else {
			int base64_str_len;
			char *base64_str;

			base64_str = (char*)php_base64_encode(outbuf, outlen, &base64_str_len);
			efree(outbuf);
			RETVAL_STRINGL(base64_str, base64_str_len, 0);
		}
	} else {
		efree(outbuf);
		RETVAL_FALSE;
	}
	if (key != (unsigned char*)password) {
		efree(key);
	}
	if (free_iv) {
		efree(iv);
	}
	EVP_CIPHER_CTX_cleanup(&cipher_ctx);
}
