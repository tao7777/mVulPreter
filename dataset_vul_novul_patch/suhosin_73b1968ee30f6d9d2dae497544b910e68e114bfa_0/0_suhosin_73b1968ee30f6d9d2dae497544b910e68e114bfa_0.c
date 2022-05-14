 char *suhosin_encrypt_single_cookie(char *name, int name_len, char *value, int value_len, char *key TSRMLS_DC)
 {
	char *buf, *buf2, *d, *d_url;
	int l;

	buf = estrndup(name, name_len);
	
 	
 	name_len = php_url_decode(buf, name_len);
	normalize_varname(buf);
	name_len = strlen(buf);
 	
 	if (SUHOSIN_G(cookie_plainlist)) {
 		if (zend_hash_exists(SUHOSIN_G(cookie_plainlist), buf, name_len+1)) {
 encrypt_return_plain:
			efree(buf);
 			return estrndup(value, value_len);
 		}
 	} else if (SUHOSIN_G(cookie_cryptlist)) {
		if (!zend_hash_exists(SUHOSIN_G(cookie_cryptlist), buf, name_len+1)) {
			goto encrypt_return_plain;
 		}
 	}
 	
	buf2 = estrndup(value, value_len);
 	
 	value_len = php_url_decode(buf2, value_len);
 	
 	d = suhosin_encrypt_string(buf2, value_len, buf, name_len, key TSRMLS_CC);
 	d_url = php_url_encode(d, strlen(d), &l);
 	efree(d);
	efree(buf);
	efree(buf2);
 	return d_url;
 }
