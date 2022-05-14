 SPL_METHOD(SplFileObject, rewind)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	spl_filesystem_file_rewind(getThis(), intern TSRMLS_CC);
} /* }}} */

/* {{{ proto void SplFileObject::eof()
