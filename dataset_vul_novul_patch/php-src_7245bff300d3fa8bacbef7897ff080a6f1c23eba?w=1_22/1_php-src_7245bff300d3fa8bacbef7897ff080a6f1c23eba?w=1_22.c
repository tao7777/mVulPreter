SPL_METHOD(SplFileObject, __construct)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_bool use_include_path = 0;
	char *p1, *p2;
	char *tmp_path;
	int   tmp_path_len;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

 	intern->u.file.open_mode = NULL;
 	intern->u.file.open_mode_len = 0;
 
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|sbr!", 
 			&intern->file_name, &intern->file_name_len,
			&intern->u.file.open_mode, &intern->u.file.open_mode_len, 
			&use_include_path, &intern->u.file.zcontext) == FAILURE) {		
 		intern->u.file.open_mode = NULL;
 		intern->file_name = NULL;
 		zend_restore_error_handling(&error_handling TSRMLS_CC);
 		return;
 	}
 	if (intern->u.file.open_mode == NULL) {
 		intern->u.file.open_mode = "r";
 		intern->u.file.open_mode_len = 1;
	}

	if (spl_filesystem_file_open(intern, use_include_path, 0 TSRMLS_CC) == SUCCESS) {
		tmp_path_len = strlen(intern->u.file.stream->orig_path);

		if (tmp_path_len > 1 && IS_SLASH_AT(intern->u.file.stream->orig_path, tmp_path_len-1)) {
			tmp_path_len--;
		}

		tmp_path = estrndup(intern->u.file.stream->orig_path, tmp_path_len);

		p1 = strrchr(tmp_path, '/');
#if defined(PHP_WIN32) || defined(NETWARE)
		p2 = strrchr(tmp_path, '\\');
#else
		p2 = 0;
#endif
		if (p1 || p2) {
			intern->_path_len = (p1 > p2 ? p1 : p2) - tmp_path;
		} else {
			intern->_path_len = 0;
		}

		efree(tmp_path);

		intern->_path = estrndup(intern->u.file.stream->orig_path, intern->_path_len);
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);

} /* }}} */

/* {{{ proto void SplTempFileObject::__construct([int max_memory])
