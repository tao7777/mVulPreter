 SPL_METHOD(SplFileObject, fgets)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	if (spl_filesystem_file_read(intern, 0 TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(intern->u.file.current_line, intern->u.file.current_line_len, 1);
} /* }}} */

/* {{{ proto string SplFileObject::current()
