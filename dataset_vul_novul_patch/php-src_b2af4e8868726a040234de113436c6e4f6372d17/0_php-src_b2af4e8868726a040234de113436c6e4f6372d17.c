PHP_FUNCTION(unserialize)
{
	char *buf = NULL;
	size_t buf_len;
 	const unsigned char *p;
 	php_unserialize_data_t var_hash;
 	zval *options = NULL, *classes = NULL;
	zval *retval;
 	HashTable *class_hash = NULL;
 
 	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|a", &buf, &buf_len, &options) == FAILURE) {
		RETURN_FALSE;
	}

	if (buf_len == 0) {
		RETURN_FALSE;
	}

	p = (const unsigned char*) buf;
	PHP_VAR_UNSERIALIZE_INIT(var_hash);
	if(options != NULL) {
		classes = zend_hash_str_find(Z_ARRVAL_P(options), "allowed_classes", sizeof("allowed_classes")-1);
		if(classes && (Z_TYPE_P(classes) == IS_ARRAY || !zend_is_true(classes))) {
			ALLOC_HASHTABLE(class_hash);
			zend_hash_init(class_hash, (Z_TYPE_P(classes) == IS_ARRAY)?zend_hash_num_elements(Z_ARRVAL_P(classes)):0, NULL, NULL, 0);
		}
		if(class_hash && Z_TYPE_P(classes) == IS_ARRAY) {
			zval *entry;
			zend_string *lcname;

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(classes), entry) {
				convert_to_string_ex(entry);
				lcname = zend_string_tolower(Z_STR_P(entry));
				zend_hash_add_empty_element(class_hash, lcname);
		        zend_string_release(lcname);
			} ZEND_HASH_FOREACH_END();
 		}
 	}
 
	retval = var_tmp_var(&var_hash);
	if (!php_var_unserialize_ex(retval, &p, p + buf_len, &var_hash, class_hash)) {
 		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
 		if (class_hash) {
 			zend_hash_destroy(class_hash);
 			FREE_HASHTABLE(class_hash);
 		}
 		if (!EG(exception)) {
 			php_error_docref(NULL, E_NOTICE, "Error at offset " ZEND_LONG_FMT " of %zd bytes",
 				(zend_long)((char*)p - buf), buf_len);
 		}
 		RETURN_FALSE;
 	}

	ZVAL_COPY(return_value, retval);
 
 	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
 	if (class_hash) {
		zend_hash_destroy(class_hash);
		FREE_HASHTABLE(class_hash);
	}
}
