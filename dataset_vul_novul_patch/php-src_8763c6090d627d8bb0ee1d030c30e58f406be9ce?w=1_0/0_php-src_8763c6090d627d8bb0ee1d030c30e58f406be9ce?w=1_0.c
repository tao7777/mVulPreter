PS_SERIALIZER_DECODE_FUNC(php) /* {{{ */
{
	const char *p, *q;
	char *name;
	const char *endptr = val + vallen;
	zval *current;
 	int namelen;
 	int has_value;
 	php_unserialize_data_t var_hash;
	int skip = 0;
 
 	PHP_VAR_UNSERIALIZE_INIT(var_hash);
 
	p = val;

 	while (p < endptr) {
 		zval **tmp;
 		q = p;
		skip = 0;
 		while (*q != PS_DELIMITER) {
 			if (++q >= endptr) goto break_outer_loop;
 		}
		if (p[0] == PS_UNDEF_MARKER) {
			p++;
			has_value = 0;
		} else {
			has_value = 1;
		}

		namelen = q - p;
		name = estrndup(p, namelen);
		q++;
 
 		if (zend_hash_find(&EG(symbol_table), name, namelen + 1, (void **) &tmp) == SUCCESS) {
 			if ((Z_TYPE_PP(tmp) == IS_ARRAY && Z_ARRVAL_PP(tmp) == &EG(symbol_table)) || *tmp == PS(http_session_vars)) {
				skip = 1;
 			}
 		}
 
 		if (has_value) {
 			ALLOC_INIT_ZVAL(current);
 			if (php_var_unserialize(&current, (const unsigned char **) &q, (const unsigned char *) endptr, &var_hash TSRMLS_CC)) {
				if (!skip) {
					php_set_session_var(name, namelen, current, &var_hash  TSRMLS_CC);
				}
 			} else {
 				var_push_dtor_no_addref(&var_hash, &current);
 				efree(name);
				PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
				return FAILURE;
 			}
 			var_push_dtor_no_addref(&var_hash, &current);
 		}
		if (!skip) {
			PS_ADD_VARL(name, namelen);
		}
 skip:
 		efree(name);
 
		p = q;
	}
break_outer_loop:

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

	return SUCCESS;
}
/* }}} */
