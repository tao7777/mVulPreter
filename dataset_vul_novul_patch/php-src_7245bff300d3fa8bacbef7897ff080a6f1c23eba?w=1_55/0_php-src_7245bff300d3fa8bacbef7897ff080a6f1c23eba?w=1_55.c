static spl_filesystem_object * spl_filesystem_object_create_type(int ht, spl_filesystem_object *source, int type, zend_class_entry *ce, zval *return_value TSRMLS_DC) /* {{{ */
{
	spl_filesystem_object *intern;
	zend_bool use_include_path = 0;
	zval *arg1, *arg2;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

	switch (source->type) {
	case SPL_FS_INFO:
	case SPL_FS_FILE:
		break;
	case SPL_FS_DIR:
		if (!source->u.dir.entry.d_name[0]) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Could not open file");
			zend_restore_error_handling(&error_handling TSRMLS_CC);
			return NULL;
		}
	}

	switch (type) {
	case SPL_FS_INFO:
		ce = ce ? ce : source->info_class;

		zend_update_class_constants(ce TSRMLS_CC);

		return_value->value.obj = spl_filesystem_object_new_ex(ce, &intern TSRMLS_CC);
		Z_TYPE_P(return_value) = IS_OBJECT;

		spl_filesystem_object_get_file_name(source TSRMLS_CC);
		if (ce->constructor->common.scope != spl_ce_SplFileInfo) {
			MAKE_STD_ZVAL(arg1);
			ZVAL_STRINGL(arg1, source->file_name, source->file_name_len, 1);
			zend_call_method_with_1_params(&return_value, ce, &ce->constructor, "__construct", NULL, arg1);
			zval_ptr_dtor(&arg1);
		} else {
			intern->file_name = estrndup(source->file_name, source->file_name_len);
			intern->file_name_len = source->file_name_len;
			intern->_path = spl_filesystem_object_get_path(source, &intern->_path_len TSRMLS_CC);
			intern->_path = estrndup(intern->_path, intern->_path_len);
		}
		break;
	case SPL_FS_FILE:
		ce = ce ? ce : source->file_class;

		zend_update_class_constants(ce TSRMLS_CC);
 
 		return_value->value.obj = spl_filesystem_object_new_ex(ce, &intern TSRMLS_CC);
 		Z_TYPE_P(return_value) = IS_OBJECT;

 		spl_filesystem_object_get_file_name(source TSRMLS_CC);
 
 		if (ce->constructor->common.scope != spl_ce_SplFileObject) {
			MAKE_STD_ZVAL(arg1);
			MAKE_STD_ZVAL(arg2);
			ZVAL_STRINGL(arg1, source->file_name, source->file_name_len, 1);
			ZVAL_STRINGL(arg2, "r", 1, 1);
			zend_call_method_with_2_params(&return_value, ce, &ce->constructor, "__construct", NULL, arg1, arg2);
			zval_ptr_dtor(&arg1);
			zval_ptr_dtor(&arg2);
		} else {
			intern->file_name = source->file_name;
 			intern->file_name_len = source->file_name_len;
 			intern->_path = spl_filesystem_object_get_path(source, &intern->_path_len TSRMLS_CC);
 			intern->_path = estrndup(intern->_path, intern->_path_len);

 			intern->u.file.open_mode = "r";
 			intern->u.file.open_mode_len = 1;

			if (ht && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sbr",
					&intern->u.file.open_mode, &intern->u.file.open_mode_len,
 					&use_include_path, &intern->u.file.zcontext) == FAILURE) {
 				zend_restore_error_handling(&error_handling TSRMLS_CC);
 				intern->u.file.open_mode = NULL;
				intern->file_name = NULL;
				zval_dtor(return_value);
 				Z_TYPE_P(return_value) = IS_NULL;
 				return NULL;
 			}

 			if (spl_filesystem_file_open(intern, use_include_path, 0 TSRMLS_CC) == FAILURE) {
 				zend_restore_error_handling(&error_handling TSRMLS_CC);
 				zval_dtor(return_value);
				Z_TYPE_P(return_value) = IS_NULL;
				return NULL;
 			}
 		}
 		break;
	case SPL_FS_DIR:
 		zend_restore_error_handling(&error_handling TSRMLS_CC);
 		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Operation not supported");
 		return NULL;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);
	return NULL;
} /* }}} */
