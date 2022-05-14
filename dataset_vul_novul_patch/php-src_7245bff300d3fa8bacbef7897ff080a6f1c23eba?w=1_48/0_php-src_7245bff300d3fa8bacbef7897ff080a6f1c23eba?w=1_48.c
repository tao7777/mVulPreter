static int spl_filesystem_file_open(spl_filesystem_object *intern, int use_include_path, int silent TSRMLS_DC) /* {{{ */
{
	zval  tmp;

	intern->type = SPL_FS_FILE;

	php_stat(intern->file_name, intern->file_name_len, FS_IS_DIR, &tmp TSRMLS_CC);
	if (Z_LVAL(tmp)) {
		intern->u.file.open_mode = NULL;
		intern->file_name = NULL;
		zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "Cannot use SplFileObject with directories");
		return FAILURE;
	}

	intern->u.file.context = php_stream_context_from_zval(intern->u.file.zcontext, 0);
	intern->u.file.stream = php_stream_open_wrapper_ex(intern->file_name, intern->u.file.open_mode, (use_include_path ? USE_PATH : 0) | REPORT_ERRORS, NULL, intern->u.file.context);

	if (!intern->file_name_len || !intern->u.file.stream) {
		if (!EG(exception)) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot open file '%s'", intern->file_name_len ? intern->file_name : "");
		}
		intern->file_name = NULL; /* until here it is not a copy */
		intern->u.file.open_mode = NULL;
		return FAILURE;
	}

	if (intern->u.file.zcontext) {
		zend_list_addref(Z_RESVAL_P(intern->u.file.zcontext));
	}

	if (intern->file_name_len > 1 && IS_SLASH_AT(intern->file_name, intern->file_name_len-1)) {
		intern->file_name_len--;
	}

	intern->orig_path = estrndup(intern->u.file.stream->orig_path, strlen(intern->u.file.stream->orig_path));

	intern->file_name = estrndup(intern->file_name, intern->file_name_len);
	intern->u.file.open_mode = estrndup(intern->u.file.open_mode, intern->u.file.open_mode_len);

 	/* avoid reference counting in debug mode, thus do it manually */
 	ZVAL_RESOURCE(&intern->u.file.zresource, php_stream_get_resource_id(intern->u.file.stream));
 	Z_SET_REFCOUNT(intern->u.file.zresource, 1);

 	intern->u.file.delimiter = ',';
 	intern->u.file.enclosure = '"';
 	intern->u.file.escape = '\\';

	zend_hash_find(&intern->std.ce->function_table, "getcurrentline", sizeof("getcurrentline"), (void **) &intern->u.file.func_getCurr);

	return SUCCESS;
} /* }}} */
