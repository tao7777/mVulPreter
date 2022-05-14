static spl_filesystem_object * spl_filesystem_object_create_info(spl_filesystem_object *source, char *file_path, int file_path_len, int use_copy, zend_class_entry *ce, zval *return_value TSRMLS_DC) /* {{{ */
{
	spl_filesystem_object *intern;
	zval *arg1;
	zend_error_handling error_handling;

	if (!file_path || !file_path_len) {
#if defined(PHP_WIN32)
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot create SplFileInfo for empty path");
		if (file_path && !use_copy) {
			efree(file_path);
		}
#else
		if (file_path && !use_copy) {
			efree(file_path);
		}
		file_path_len = 1;
		file_path = "/";
#endif
		return NULL;
	}

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

	ce = ce ? ce : source->info_class;

	zend_update_class_constants(ce TSRMLS_CC);

	return_value->value.obj = spl_filesystem_object_new_ex(ce, &intern TSRMLS_CC);
	Z_TYPE_P(return_value) = IS_OBJECT;

	if (ce->constructor->common.scope != spl_ce_SplFileInfo) {
		MAKE_STD_ZVAL(arg1);
		ZVAL_STRINGL(arg1, file_path, file_path_len, use_copy);
		zend_call_method_with_1_params(&return_value, ce, &ce->constructor, "__construct", NULL, arg1);
		zval_ptr_dtor(&arg1);
 	} else {
 		spl_filesystem_info_set_filename(intern, file_path, file_path_len, use_copy TSRMLS_CC);
 	}
 	zend_restore_error_handling(&error_handling TSRMLS_CC);
 	return intern;
 } /* }}} */
