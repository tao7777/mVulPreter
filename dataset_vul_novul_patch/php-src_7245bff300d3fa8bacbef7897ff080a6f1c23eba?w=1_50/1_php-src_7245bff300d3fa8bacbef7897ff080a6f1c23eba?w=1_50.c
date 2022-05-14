 static int spl_filesystem_file_read_csv(spl_filesystem_object *intern, char delimiter, char enclosure, char escape, zval *return_value TSRMLS_DC) /* {{{ */
 {
 	int ret = SUCCESS;
 	do {
 		ret = spl_filesystem_file_read(intern, 1 TSRMLS_CC);
 	} while (ret == SUCCESS && !intern->u.file.current_line_len && SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_SKIP_EMPTY));
 	if (ret == SUCCESS) {
 		size_t buf_len = intern->u.file.current_line_len;
 		char *buf = estrndup(intern->u.file.current_line, buf_len);

		if (intern->u.file.current_zval) {
			zval_ptr_dtor(&intern->u.file.current_zval);
		}
		ALLOC_INIT_ZVAL(intern->u.file.current_zval);

		php_fgetcsv(intern->u.file.stream, delimiter, enclosure, escape, buf_len, buf, intern->u.file.current_zval TSRMLS_CC);
		if (return_value) {
			if (Z_TYPE_P(return_value) != IS_NULL) {
				zval_dtor(return_value);
				ZVAL_NULL(return_value);
			}
			ZVAL_ZVAL(return_value, intern->u.file.current_zval, 1, 0);
		}
	}
	return ret;
}
/* }}} */
