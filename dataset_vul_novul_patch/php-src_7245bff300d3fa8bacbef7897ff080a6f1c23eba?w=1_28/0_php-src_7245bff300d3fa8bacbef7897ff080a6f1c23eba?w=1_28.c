 SPL_METHOD(SplFileObject, current)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	if (!intern->u.file.current_line && !intern->u.file.current_zval) {
		spl_filesystem_file_read_line(getThis(), intern, 1 TSRMLS_CC);
	}
	if (intern->u.file.current_line && (!SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV) || !intern->u.file.current_zval)) {
		RETURN_STRINGL(intern->u.file.current_line, intern->u.file.current_line_len, 1);
	} else if (intern->u.file.current_zval) {
		RETURN_ZVAL(intern->u.file.current_zval, 1, 0);
	}
	RETURN_FALSE;
} /* }}} */

/* {{{ proto int SplFileObject::key()
