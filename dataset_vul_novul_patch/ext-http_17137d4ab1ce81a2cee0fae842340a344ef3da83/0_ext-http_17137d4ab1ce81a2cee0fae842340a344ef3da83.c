static void merge_param(HashTable *params, zval *zdata, zval ***current_param, zval ***current_args TSRMLS_DC)
{
	zval **ptr, **zdata_ptr;
	php_http_array_hashkey_t hkey = php_http_array_hashkey_init(0);

#if 0
	{
		zval tmp;
		INIT_PZVAL_ARRAY(&tmp, params);
		fprintf(stderr, "params = ");
		zend_print_zval_r(&tmp, 1 TSRMLS_CC);
		fprintf(stderr, "\n");
	}
#endif

	hkey.type = zend_hash_get_current_key_ex(Z_ARRVAL_P(zdata), &hkey.str, &hkey.len, &hkey.num, hkey.dup, NULL);

	if ((hkey.type == HASH_KEY_IS_STRING && !zend_hash_exists(params, hkey.str, hkey.len))
	||	(hkey.type == HASH_KEY_IS_LONG && !zend_hash_index_exists(params, hkey.num))
	) {
		zval *tmp, *arg, **args;

		/* create the entry if it doesn't exist */
		zend_hash_get_current_data(Z_ARRVAL_P(zdata), (void *) &ptr);
		Z_ADDREF_PP(ptr);
		MAKE_STD_ZVAL(tmp);
		array_init(tmp);
		add_assoc_zval_ex(tmp, ZEND_STRS("value"), *ptr);

		MAKE_STD_ZVAL(arg);
		array_init(arg);
		zend_hash_update(Z_ARRVAL_P(tmp), "arguments", sizeof("arguments"), (void *) &arg, sizeof(zval *), (void *) &args);
		*current_args = args;

		if (hkey.type == HASH_KEY_IS_STRING) {
			zend_hash_update(params, hkey.str, hkey.len, (void *) &tmp, sizeof(zval *), (void *) &ptr);
		} else {
			zend_hash_index_update(params, hkey.num, (void *) &tmp, sizeof(zval *), (void *) &ptr);
		}
	} else {
		/* merge */
		if (hkey.type == HASH_KEY_IS_STRING) {
			zend_hash_find(params, hkey.str, hkey.len, (void *) &ptr);
		} else {
			zend_hash_index_find(params, hkey.num, (void *) &ptr);
		}

		zdata_ptr = &zdata;

		if (Z_TYPE_PP(ptr) == IS_ARRAY
		&&	SUCCESS == zend_hash_find(Z_ARRVAL_PP(ptr), "value", sizeof("value"), (void *) &ptr)
		&&	SUCCESS == zend_hash_get_current_data(Z_ARRVAL_PP(zdata_ptr), (void *) &zdata_ptr)
		) {
			/*
			 * params = [arr => [value => [0 => 1]]]
			 *                            ^- ptr
			 * zdata  = [arr => [0 => NULL]]
			 *                  ^- zdata_ptr
			 */
			zval **test_ptr;

 			while (Z_TYPE_PP(zdata_ptr) == IS_ARRAY
 			&&	SUCCESS == zend_hash_get_current_data(Z_ARRVAL_PP(zdata_ptr), (void *) &test_ptr)
 			) {
				if (Z_TYPE_PP(test_ptr) == IS_ARRAY && Z_TYPE_PP(ptr) == IS_ARRAY) {
 
 					/* now find key in ptr */
 					if (HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(Z_ARRVAL_PP(zdata_ptr), &hkey.str, &hkey.len, &hkey.num, hkey.dup, NULL)) {
						if (SUCCESS == zend_hash_find(Z_ARRVAL_PP(ptr), hkey.str, hkey.len, (void *) &ptr)) {
							zdata_ptr = test_ptr;
						} else {
							Z_ADDREF_PP(test_ptr);
							zend_hash_update(Z_ARRVAL_PP(ptr), hkey.str, hkey.len, (void *) test_ptr, sizeof(zval *), (void *) &ptr);
							break;
						}
					} else {
						if (SUCCESS == zend_hash_index_find(Z_ARRVAL_PP(ptr), hkey.num, (void *) &ptr)) {
							zdata_ptr = test_ptr;
						} else if (hkey.num) {
							Z_ADDREF_PP(test_ptr);
							zend_hash_index_update(Z_ARRVAL_PP(ptr), hkey.num, (void *) test_ptr, sizeof(zval *), (void *) &ptr);
							break;
						} else {
							Z_ADDREF_PP(test_ptr);
							zend_hash_next_index_insert(Z_ARRVAL_PP(ptr), (void *) test_ptr, sizeof(zval *), (void *) &ptr);
							break;
						}
					}
				} else {
					/* this is the leaf */
					Z_ADDREF_PP(test_ptr);
					if (Z_TYPE_PP(ptr) != IS_ARRAY) {
						zval_dtor(*ptr);
						array_init(*ptr);
					}
					if (HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(Z_ARRVAL_PP(zdata_ptr), &hkey.str, &hkey.len, &hkey.num, hkey.dup, NULL)) {
						zend_hash_update(Z_ARRVAL_PP(ptr), hkey.str, hkey.len, (void *) test_ptr, sizeof(zval *), (void *) &ptr);
					} else if (hkey.num) {
						zend_hash_index_update(Z_ARRVAL_PP(ptr), hkey.num, (void *) test_ptr, sizeof(zval *), (void *) &ptr);
					} else {
						zend_hash_next_index_insert(Z_ARRVAL_PP(ptr), (void *) test_ptr, sizeof(zval *), (void *) &ptr);
					}
					break;
				}
			}

		}
	}

	/* bubble up */
	while (Z_TYPE_PP(ptr) == IS_ARRAY && SUCCESS == zend_hash_get_current_data(Z_ARRVAL_PP(ptr), (void *) &ptr));
	*current_param = ptr;
}
