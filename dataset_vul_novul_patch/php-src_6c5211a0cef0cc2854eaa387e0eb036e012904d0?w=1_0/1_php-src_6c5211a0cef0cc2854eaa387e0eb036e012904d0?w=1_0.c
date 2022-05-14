PHP_FUNCTION(mcrypt_generic_init)
{
	char *key, *iv;
	int key_len, iv_len;
	zval *mcryptind;
	unsigned char *key_s, *iv_s;
 	int max_key_size, key_size, iv_size;
 	php_mcrypt *pm;
 	int result = 0;
 	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &mcryptind, &key, &key_len, &iv, &iv_len) == FAILURE) {
 		return;
 	}

	ZEND_FETCH_RESOURCE(pm, php_mcrypt *, &mcryptind, -1, "MCrypt", le_mcrypt);

	max_key_size = mcrypt_enc_get_key_size(pm->td);
	iv_size = mcrypt_enc_get_iv_size(pm->td);

	if (key_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Key size is 0");
	}

	key_s = emalloc(key_len);
	memset(key_s, 0, key_len);

	iv_s = emalloc(iv_size + 1);
	memset(iv_s, 0, iv_size + 1);

	if (key_len > max_key_size) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Key size too large; supplied length: %d, max: %d", key_len, max_key_size);
		key_size = max_key_size;
	} else {
		key_size = key_len;
	}
	memcpy(key_s, key, key_len);

	if (iv_len != iv_size) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Iv size incorrect; supplied length: %d, needed: %d", iv_len, iv_size);
		if (iv_len > iv_size) {
			iv_len = iv_size;
		}
	}
	memcpy(iv_s, iv, iv_len);

	mcrypt_generic_deinit(pm->td);
	result = mcrypt_generic_init(pm->td, key_s, key_size, iv_s);

	/* If this function fails, close the mcrypt module to prevent crashes
	 * when further functions want to access this resource */
	if (result < 0) {
		zend_list_delete(Z_LVAL_P(mcryptind));
		switch (result) {
			case -3:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Key length incorrect");
				break;
			case -4:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Memory allocation error");
				break;
			case -1:
			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown error");
				break;
		}
	} else {
		pm->init = 1;
	}
	RETVAL_LONG(result);

	efree(iv_s);
	efree(key_s);
}
