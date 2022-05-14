 SPL_METHOD(SplFileObject, valid)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		RETURN_BOOL(intern->u.file.current_line || intern->u.file.current_zval);
	} else {
		RETVAL_BOOL(!php_stream_eof(intern->u.file.stream));
	}
} /* }}} */

/* {{{ proto string SplFileObject::fgets()
