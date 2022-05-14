PHP_FUNCTION(mcrypt_encrypt)
{
 	zval **mode;
 	char *cipher, *key, *data, *iv = NULL;
 	int cipher_len, key_len, data_len, iv_len = 0;
 	MCRYPT_GET_CRYPT_ARGS
 	convert_to_string_ex(mode);
 
 	php_mcrypt_do_crypt(cipher, key, key_len, data, data_len, Z_STRVAL_PP(mode), iv, iv_len, ZEND_NUM_ARGS(), MCRYPT_ENCRYPT, return_value TSRMLS_CC);
}
