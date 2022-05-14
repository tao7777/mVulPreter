SPL_METHOD(SplFileInfo, getRealPath)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	char buff[MAXPATHLEN];
 	char *filename;
 	zend_error_handling error_handling;
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

 	if (intern->type == SPL_FS_DIR && !intern->file_name && intern->u.dir.entry.d_name[0]) {
 		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
 	}
 	if (intern->orig_path) {
 		filename = intern->orig_path;
	} else { 
 		filename = intern->file_name;
 	}
 

	if (filename && VCWD_REALPATH(filename, buff)) {
#ifdef ZTS
		if (VCWD_ACCESS(buff, F_OK)) {
			RETVAL_FALSE;
		} else
#endif
		RETVAL_STRING(buff, 1);
	} else {
		RETVAL_FALSE;
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
}
