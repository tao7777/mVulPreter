static int spl_filesystem_file_call(spl_filesystem_object *intern, zend_function *func_ptr, int pass_num_args, zval *return_value, zval *arg2 TSRMLS_DC) /* {{{ */
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcic;
	zval z_fname;
	zval * zresource_ptr = &intern->u.file.zresource, *retval;
	int result;
	int num_args = pass_num_args + (arg2 ? 2 : 1);

 	zval ***params = (zval***)safe_emalloc(num_args, sizeof(zval**), 0);
 
 	params[0] = &zresource_ptr;
 	if (arg2) {
 		params[1] = &arg2;
 	}

	zend_get_parameters_array_ex(pass_num_args, params+(arg2 ? 2 : 1));

	ZVAL_STRING(&z_fname, func_ptr->common.function_name, 0);

	fci.size = sizeof(fci);
	fci.function_table = EG(function_table);
	fci.object_ptr = NULL;
	fci.function_name = &z_fname;
	fci.retval_ptr_ptr = &retval;
	fci.param_count = num_args;
	fci.params = params;
	fci.no_separation = 1;
	fci.symbol_table = NULL;

	fcic.initialized = 1;
	fcic.function_handler = func_ptr;
	fcic.calling_scope = NULL;
	fcic.called_scope = NULL;
 	fcic.object_ptr = NULL;
 
 	result = zend_call_function(&fci, &fcic TSRMLS_CC);
 	if (result == FAILURE) {
 		RETVAL_FALSE;
 	} else {
		ZVAL_ZVAL(return_value, retval, 1, 1);
	}

	efree(params);
	return result;
} /* }}} */
