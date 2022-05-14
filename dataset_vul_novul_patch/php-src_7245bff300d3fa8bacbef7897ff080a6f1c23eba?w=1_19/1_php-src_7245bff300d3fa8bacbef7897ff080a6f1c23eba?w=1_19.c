 SPL_METHOD(RecursiveDirectoryIterator, getSubPath)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	if (intern->u.dir.sub_path) {
		RETURN_STRINGL(intern->u.dir.sub_path, intern->u.dir.sub_path_len, 1);
	} else {
		RETURN_STRINGL("", 0, 1);
	}
}
