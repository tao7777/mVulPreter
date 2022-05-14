 char *suhosin_decrypt_single_cookie(char *name, int name_len, char *value, int value_len, char *key, char **where TSRMLS_DC)
 {
	char buffer[4096];
    char buffer2[4096];
     int o_name_len = name_len;
	char *buf = buffer, *buf2 = buffer2, *d, *d_url;
 	int l;
 
	if (name_len > sizeof(buffer)-2) {
		buf = estrndup(name, name_len);
	} else {
		memcpy(buf, name, name_len);
		buf[name_len] = 0;
	}
 	name_len = php_url_decode(buf, name_len);
    normalize_varname(buf);
    name_len = strlen(buf);
 	
 	if (SUHOSIN_G(cookie_plainlist)) {
 		if (zend_hash_exists(SUHOSIN_G(cookie_plainlist), buf, name_len+1)) {
 decrypt_return_plain:
			if (buf != buffer) {
				efree(buf);
			}
             memcpy(*where, name, o_name_len);
             *where += o_name_len;
             **where = '='; *where +=1;
	        memcpy(*where, value, value_len);
	        *where += value_len;
			return *where;
		}
	} else if (SUHOSIN_G(cookie_cryptlist)) {
		if (!zend_hash_exists(SUHOSIN_G(cookie_cryptlist), buf, name_len+1)) {
			goto decrypt_return_plain;
		}
 	}
 	
 	
	if (strlen(value) <= sizeof(buffer2)-2) {
		memcpy(buf2, value, value_len);
		buf2[value_len] = 0;
	} else {
		buf2 = estrndup(value, value_len);
	}
 	
 	value_len = php_url_decode(buf2, value_len);
 	
	d = suhosin_decrypt_string(buf2, value_len, buf, name_len, key, &l, SUHOSIN_G(cookie_checkraddr) TSRMLS_CC);
    if (d == NULL) {
        goto skip_cookie;
    }
	d_url = php_url_encode(d, l, &l);
	efree(d);
    memcpy(*where, name, o_name_len);
    *where += o_name_len;
    **where = '=';*where += 1;
	memcpy(*where, d_url, l);
 	*where += l;
 	efree(d_url);
 skip_cookie:
	if (buf != buffer) {
		efree(buf);
	}
	if (buf2 != buffer2) {
		efree(buf2);
	}
 	return *where;
 }
