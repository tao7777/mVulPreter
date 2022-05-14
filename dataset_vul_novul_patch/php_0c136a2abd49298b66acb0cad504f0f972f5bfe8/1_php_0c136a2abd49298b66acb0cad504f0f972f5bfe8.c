int basic_authentication(zval* this_ptr, smart_str* soap_headers TSRMLS_DC)
{
	zval **login, **password;

        zval **login, **password;
 
        if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_login", sizeof("_login"), (void **)&login) == SUCCESS &&
                       !zend_hash_exists(Z_OBJPROP_P(this_ptr), "_digest", sizeof("_digest"))) {
                unsigned char* buf;
                int len;
                smart_str auth = {0};
 
                smart_str_appendl(&auth, Z_STRVAL_PP(login), Z_STRLEN_PP(login));
                smart_str_appendc(&auth, ':');
               if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_password", sizeof("_password"), (void **)&password) == SUCCESS) {
                        smart_str_appendl(&auth, Z_STRVAL_PP(password), Z_STRLEN_PP(password));
                }
                smart_str_0(&auth);
		efree(buf);
		smart_str_free(&auth);
		return 1;
	}
	return 0;
}
