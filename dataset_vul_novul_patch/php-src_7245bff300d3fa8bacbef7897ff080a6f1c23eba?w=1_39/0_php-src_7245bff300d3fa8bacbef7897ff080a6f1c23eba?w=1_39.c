SPL_METHOD(SplFileObject, fwrite)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *str;
	int str_len;
	long length = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &str_len, &length) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() > 1) {
		str_len = MAX(0, MIN(length, str_len));
	}
	if (!str_len) {
		RETURN_LONG(0);
	}

	RETURN_LONG(php_stream_write(intern->u.file.stream, str, str_len));
} /* }}} */

SPL_METHOD(SplFileObject, fread)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long length = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &length) == FAILURE) {
		return;
	}

	if (length <= 0) {
 		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter must be greater than 0");
 		RETURN_FALSE;
 	}
	if (length > INT_MAX) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter must be no more than %d", INT_MAX);
		RETURN_FALSE;
	}
 
 	Z_STRVAL_P(return_value) = emalloc(length + 1);
 	Z_STRLEN_P(return_value) = php_stream_read(intern->u.file.stream, Z_STRVAL_P(return_value), length);

	/* needed because recv/read/gzread doesnt put a null at the end*/
	Z_STRVAL_P(return_value)[Z_STRLEN_P(return_value)] = 0;
	Z_TYPE_P(return_value) = IS_STRING;
}

/* {{{ proto bool SplFileObject::fstat()
