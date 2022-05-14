 SPL_METHOD(SplFileObject, next)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	spl_filesystem_file_free_line(intern TSRMLS_CC);
	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		spl_filesystem_file_read_line(getThis(), intern, 1 TSRMLS_CC);
	}
	intern->u.file.current_line_num++;
} /* }}} */

/* {{{ proto void SplFileObject::setFlags(int flags)
