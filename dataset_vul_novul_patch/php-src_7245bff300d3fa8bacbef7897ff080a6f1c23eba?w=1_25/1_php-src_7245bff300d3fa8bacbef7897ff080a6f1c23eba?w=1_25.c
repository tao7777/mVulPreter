 SPL_METHOD(SplFileObject, eof)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	RETURN_BOOL(php_stream_eof(intern->u.file.stream));
} /* }}} */

/* {{{ proto void SplFileObject::valid()
