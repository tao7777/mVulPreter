int proxy_authentication(zval* this_ptr, smart_str* soap_headers TSRMLS_DC)
 {
        zval **login, **password;
 
       if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_proxy_login", sizeof("_proxy_login"), (void **)&login) == SUCCESS &&
           Z_TYPE_PP(login) == IS_STRING) {
                unsigned char* buf;
                int len;
                smart_str auth = {0};
 
                smart_str_appendl(&auth, Z_STRVAL_PP(login), Z_STRLEN_PP(login));
                smart_str_appendc(&auth, ':');
               if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_proxy_password", sizeof("_proxy_password"), (void **)&password) == SUCCESS &&
                   Z_TYPE_PP(password) == IS_STRING) {
                        smart_str_appendl(&auth, Z_STRVAL_PP(password), Z_STRLEN_PP(password));
                }
                smart_str_0(&auth);
		smart_str_appendl(soap_headers, (char*)buf, len);
		smart_str_append_const(soap_headers, "\r\n");
		efree(buf);
		smart_str_free(&auth);
		return 1;
	}
	return 0;
}
