PHP_FUNCTION(mcrypt_get_block_size)
 {
 	char *cipher;
 	char *module;
	int   cipher_len, module_len; 
 	char *cipher_dir_string;
 	char *module_dir_string;
 	MCRYPT td;

	MCRYPT_GET_INI

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
 		&cipher, &cipher_len, &module, &module_len) == FAILURE) {
 		return;
 	}
 	td = mcrypt_module_open(cipher, cipher_dir_string, module, module_dir_string);
 	if (td != MCRYPT_FAILED) {
 		RETVAL_LONG(mcrypt_enc_get_block_size(td));
		mcrypt_module_close(td);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, MCRYPT_OPEN_MODULE_FAILED);
		RETURN_FALSE;
	}
}
