static int spl_filesystem_file_is_empty_line(spl_filesystem_object *intern TSRMLS_DC) /* {{{ */
{
	if (intern->u.file.current_line) {
		return intern->u.file.current_line_len == 0;
	} else if (intern->u.file.current_zval) {
		switch(Z_TYPE_P(intern->u.file.current_zval)) {
		case IS_STRING:
			return Z_STRLEN_P(intern->u.file.current_zval) == 0;
		case IS_ARRAY:
 			if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV)
 			&& zend_hash_num_elements(Z_ARRVAL_P(intern->u.file.current_zval)) == 1) {
 				zval ** first = Z_ARRVAL_P(intern->u.file.current_zval)->pListHead->pData;
 				return Z_TYPE_PP(first) == IS_STRING && Z_STRLEN_PP(first) == 0;
 			}
 			return zend_hash_num_elements(Z_ARRVAL_P(intern->u.file.current_zval)) == 0;
		case IS_NULL:
			return 1;
		default:
			return 0;
		}
	} else {
		return 1;
	}
}
/* }}} */
