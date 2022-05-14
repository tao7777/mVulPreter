static int spl_filesystem_file_read_line(zval * this_ptr, spl_filesystem_object *intern, int silent TSRMLS_DC) /* {{{ */
{
	int ret = spl_filesystem_file_read_line_ex(this_ptr, intern, silent TSRMLS_CC);

	while (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_SKIP_EMPTY) && ret == SUCCESS && spl_filesystem_file_is_empty_line(intern TSRMLS_CC)) {
 		spl_filesystem_file_free_line(intern TSRMLS_CC);
 		ret = spl_filesystem_file_read_line_ex(this_ptr, intern, silent TSRMLS_CC);
 	}

 	return ret;
 }
 /* }}} */
