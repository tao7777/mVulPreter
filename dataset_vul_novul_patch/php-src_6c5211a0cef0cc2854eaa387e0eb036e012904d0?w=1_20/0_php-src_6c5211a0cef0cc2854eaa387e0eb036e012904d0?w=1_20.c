PHP_FUNCTION(mcrypt_ofb)
{
 	zval **mode;
 	char *cipher, *key, *data, *iv = NULL;
 	int cipher_len, key_len, data_len, iv_len = 0;

 	MCRYPT_GET_CRYPT_ARGS
 
 	convert_to_long_ex(mode);

	php_mcrypt_do_crypt(cipher, key, key_len, data, data_len, "ofb", iv, iv_len, ZEND_NUM_ARGS(), Z_LVAL_PP(mode), return_value TSRMLS_CC);
}
