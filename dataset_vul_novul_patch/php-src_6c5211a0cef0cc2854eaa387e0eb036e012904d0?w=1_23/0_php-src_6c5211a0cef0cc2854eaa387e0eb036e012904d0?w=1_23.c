static void php_mcrypt_do_crypt(char* cipher, const char *key, int key_len, const char *data, int data_len, char *mode, const char *iv, int iv_len, int argc, int dencrypt, zval* return_value TSRMLS_DC) /* {{{ */
{
	char *cipher_dir_string;
	char *module_dir_string;
	int block_size, max_key_length, use_key_length, i, count, iv_size;
	unsigned long int data_size;
	int *key_length_sizes;
	char *key_s = NULL, *iv_s;
	char *data_s;
	MCRYPT td;

	MCRYPT_GET_INI

	td = mcrypt_module_open(cipher, cipher_dir_string, mode, module_dir_string);
	if (td == MCRYPT_FAILED) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, MCRYPT_OPEN_MODULE_FAILED);
		RETURN_FALSE;
	}
	/* Checking for key-length */
	max_key_length = mcrypt_enc_get_key_size(td);
	if (key_len > max_key_length) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Size of key is too large for this algorithm");
	}
	key_length_sizes = mcrypt_enc_get_supported_key_sizes(td, &count);
	if (count == 0 && key_length_sizes == NULL) { /* all lengths 1 - k_l_s = OK */
		use_key_length = key_len;
		key_s = emalloc(use_key_length);
		memset(key_s, 0, use_key_length);
		memcpy(key_s, key, use_key_length);
	} else if (count == 1) {  /* only m_k_l = OK */
		key_s = emalloc(key_length_sizes[0]);
		memset(key_s, 0, key_length_sizes[0]);
		memcpy(key_s, key, MIN(key_len, key_length_sizes[0]));
		use_key_length = key_length_sizes[0];
 	} else { /* dertermine smallest supported key > length of requested key */
 		use_key_length = max_key_length; /* start with max key length */
 		for (i = 0; i < count; i++) {
			if (key_length_sizes[i] >= key_len &&
 				key_length_sizes[i] < use_key_length)
 			{
 				use_key_length = key_length_sizes[i];
			}
		}
		key_s = emalloc(use_key_length);
		memset(key_s, 0, use_key_length);
 		memcpy(key_s, key, MIN(key_len, use_key_length));
 	}
 	mcrypt_free (key_length_sizes);

 	/* Check IV */
 	iv_s = NULL;
 	iv_size = mcrypt_enc_get_iv_size (td);

 	/* IV is required */
 	if (mcrypt_enc_mode_has_iv(td) == 1) {
 		if (argc == 5) {
			if (iv_size != iv_len) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, MCRYPT_IV_WRONG_SIZE);
			} else {
				iv_s = emalloc(iv_size + 1);
				memcpy(iv_s, iv, iv_size);
			}
		} else if (argc == 4) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempt to use an empty IV, which is NOT recommend");
			iv_s = emalloc(iv_size + 1);
			memset(iv_s, 0, iv_size + 1);
		}
	}

	/* Check blocksize */
	if (mcrypt_enc_is_block_mode(td) == 1) { /* It's a block algorithm */
		block_size = mcrypt_enc_get_block_size(td);
		data_size = (((data_len - 1) / block_size) + 1) * block_size;
		data_s = emalloc(data_size);
		memset(data_s, 0, data_size);
		memcpy(data_s, data, data_len);
	} else { /* It's not a block algorithm */
		data_size = data_len;
		data_s = emalloc(data_size);
		memset(data_s, 0, data_size);
		memcpy(data_s, data, data_len);
	}

	if (mcrypt_generic_init(td, key_s, use_key_length, iv_s) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Mcrypt initialisation failed");
		RETURN_FALSE;
	}
	if (dencrypt == MCRYPT_ENCRYPT) {
		mcrypt_generic(td, data_s, data_size);
 	} else {
 		mdecrypt_generic(td, data_s, data_size);
 	}

 	RETVAL_STRINGL(data_s, data_size, 1);
 
 	/* freeing vars */
	mcrypt_generic_end(td);
	if (key_s != NULL) {
		efree (key_s);
	}
	if (iv_s != NULL) {
		efree (iv_s);
	}
	efree (data_s);
}
/* }}} */
