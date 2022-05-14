 SPL_METHOD(SplFileObject, key)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

/*	Do not read the next line to support correct counting with fgetc()
	if (!intern->current_line) {
		spl_filesystem_file_read_line(getThis(), intern, 1 TSRMLS_CC);
	} */
	RETURN_LONG(intern->u.file.current_line_num);
} /* }}} */

/* {{{ proto void SplFileObject::next()
