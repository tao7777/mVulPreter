 SPL_METHOD(DirectoryIterator, key)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	if (intern->u.dir.dirp) {
		RETURN_LONG(intern->u.dir.index);
	} else {
		RETURN_FALSE;
	}
}
