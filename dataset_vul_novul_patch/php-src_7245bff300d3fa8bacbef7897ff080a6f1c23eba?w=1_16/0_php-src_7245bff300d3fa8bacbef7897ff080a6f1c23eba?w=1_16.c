SPL_METHOD(SplFileInfo, getPathInfo)
{
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	zend_class_entry *ce = intern->info_class;
 	zend_error_handling error_handling;

 	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling TSRMLS_CC);
 
 	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|C", &ce) == SUCCESS) {
		int path_len;
		char *path = spl_filesystem_object_get_pathname(intern, &path_len TSRMLS_CC);
		if (path) {
			char *dpath = estrndup(path, path_len);
			path_len = php_dirname(dpath, path_len);
			spl_filesystem_object_create_info(intern, dpath, path_len, 1, ce, return_value TSRMLS_CC);
			efree(dpath);
		}
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
}
