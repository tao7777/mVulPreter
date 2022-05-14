static int spl_filesystem_file_read(spl_filesystem_object *intern, int silent TSRMLS_DC) /* {{{ */
{
	char *buf;
	size_t line_len = 0;
 	long line_add = (intern->u.file.current_line || intern->u.file.current_zval) ? 1 : 0;
 
 	spl_filesystem_file_free_line(intern TSRMLS_CC);
 	if (php_stream_eof(intern->u.file.stream)) {
 		if (!silent) {
 			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot read from file %s", intern->file_name);
		}
		return FAILURE;
	}

	if (intern->u.file.max_line_len > 0) {
		buf = safe_emalloc((intern->u.file.max_line_len + 1), sizeof(char), 0);
		if (php_stream_get_line(intern->u.file.stream, buf, intern->u.file.max_line_len + 1, &line_len) == NULL) {
			efree(buf);
			buf = NULL;
		} else {
			buf[line_len] = '\0';
		}
	} else {
		buf = php_stream_get_line(intern->u.file.stream, NULL, 0, &line_len);
	}

	if (!buf) {
		intern->u.file.current_line = estrdup("");
		intern->u.file.current_line_len = 0;
	} else {
		if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_DROP_NEW_LINE)) {
 			line_len = strcspn(buf, "\r\n");
 			buf[line_len] = '\0';
 		}
 		intern->u.file.current_line = buf;
 		intern->u.file.current_line_len = line_len;
 	}
	intern->u.file.current_line_num += line_add;

	return SUCCESS;
} /* }}} */
