zend_op_array *compile_string(zval *source_string, char *filename TSRMLS_DC)
{
	zend_lex_state original_lex_state;
	zend_op_array *op_array = (zend_op_array *) emalloc(sizeof(zend_op_array));
	zend_op_array *original_active_op_array = CG(active_op_array);
	zend_op_array *retval;
	zval tmp;
	int compiler_result;
	zend_bool original_in_compilation = CG(in_compilation);

	if (source_string->value.str.len==0) {
		efree(op_array);
		return NULL;
	}

	CG(in_compilation) = 1;

	tmp = *source_string;
	zval_copy_ctor(&tmp);
	convert_to_string(&tmp);
	source_string = &tmp;

	zend_save_lexical_state(&original_lex_state TSRMLS_CC);
	if (zend_prepare_string_for_scanning(source_string, filename TSRMLS_CC)==FAILURE) {
		efree(op_array);
		retval = NULL;
	} else {
		zend_bool orig_interactive = CG(interactive);

		CG(interactive) = 0;
		init_op_array(op_array, ZEND_EVAL_CODE, INITIAL_OP_ARRAY_SIZE TSRMLS_CC);
		CG(interactive) = orig_interactive;
		CG(active_op_array) = op_array;
		zend_stack_push(&CG(context_stack), (void *) &CG(context), sizeof(CG(context)));
		zend_init_compiler_context(TSRMLS_C);
		BEGIN(ST_IN_SCRIPTING);
		compiler_result = zendparse(TSRMLS_C);

		if (SCNG(script_filtered)) {
			efree(SCNG(script_filtered));
 			SCNG(script_filtered) = NULL;
 		}
 
		if (compiler_result != 0) {
 			CG(active_op_array) = original_active_op_array;
 			CG(unclean_shutdown)=1;
 			destroy_op_array(op_array TSRMLS_CC);
			efree(op_array);
			retval = NULL;
		} else {
			zend_do_return(NULL, 0 TSRMLS_CC);
			CG(active_op_array) = original_active_op_array;
			pass_two(op_array TSRMLS_CC);
			zend_release_labels(0 TSRMLS_CC);
			retval = op_array;
		}
	}
	zend_restore_lexical_state(&original_lex_state TSRMLS_CC);
	zval_dtor(&tmp);
	CG(in_compilation) = original_in_compilation;
	return retval;
}
