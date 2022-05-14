 SPL_METHOD(SplFileObject, getMaxLineLen)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	RETURN_LONG((long)intern->u.file.max_line_len);
} /* }}} */

/* {{{ proto bool SplFileObject::hasChildren()
