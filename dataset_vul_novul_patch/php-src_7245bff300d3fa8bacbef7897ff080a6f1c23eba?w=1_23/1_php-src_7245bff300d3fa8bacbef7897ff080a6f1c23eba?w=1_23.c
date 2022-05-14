SPL_METHOD(SplTempFileObject, __construct)
{
	long max_memory = PHP_STREAM_MAX_MEM;
	char tmp_fname[48];
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &max_memory) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	if (max_memory < 0) {
		intern->file_name = "php://memory";
		intern->file_name_len = 12;
	} else if (ZEND_NUM_ARGS()) {
		intern->file_name_len = slprintf(tmp_fname, sizeof(tmp_fname), "php://temp/maxmemory:%ld", max_memory);
		intern->file_name = tmp_fname;
	} else {
		intern->file_name = "php://temp";
		intern->file_name_len = 10;
	}
 	intern->u.file.open_mode = "wb";
 	intern->u.file.open_mode_len = 1;
 	intern->u.file.zcontext = NULL;
 	if (spl_filesystem_file_open(intern, 0, 0 TSRMLS_CC) == SUCCESS) {
 		intern->_path_len = 0;
 		intern->_path = estrndup("", 0);
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);
} /* }}} */

/* {{{ proto void SplFileObject::rewind()
