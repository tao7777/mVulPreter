 SPL_METHOD(SplFileInfo, __construct)
{
	spl_filesystem_object *intern;
	char *path;
	int len;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &len) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
 	}
 
 	intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	spl_filesystem_info_set_filename(intern, path, len, 1 TSRMLS_CC);
 
 	zend_restore_error_handling(&error_handling TSRMLS_CC);
 	/* intern->type = SPL_FS_INFO; already set */
 }
