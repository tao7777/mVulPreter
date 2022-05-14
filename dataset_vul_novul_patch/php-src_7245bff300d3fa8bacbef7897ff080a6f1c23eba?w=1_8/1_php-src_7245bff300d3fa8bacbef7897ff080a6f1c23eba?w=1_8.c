 SPL_METHOD(FilesystemIterator, key)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	if (SPL_FILE_DIR_KEY(intern, SPL_FILE_DIR_KEY_AS_FILENAME)) {
		RETURN_STRING(intern->u.dir.entry.d_name, 1);
	} else {
		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
		RETURN_STRINGL(intern->file_name, intern->file_name_len, 1);
	}
}
