 SPL_METHOD(FilesystemIterator, current)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	if (SPL_FILE_DIR_CURRENT(intern, SPL_FILE_DIR_CURRENT_AS_PATHNAME)) {
		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
		RETURN_STRINGL(intern->file_name, intern->file_name_len, 1);
	} else if (SPL_FILE_DIR_CURRENT(intern, SPL_FILE_DIR_CURRENT_AS_FILEINFO)) {
		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
		spl_filesystem_object_create_type(0, intern, SPL_FS_INFO, NULL, return_value TSRMLS_CC);
	} else {
		RETURN_ZVAL(getThis(), 1, 0);
		/*RETURN_STRING(intern->u.dir.entry.d_name, 1);*/
	}
}
