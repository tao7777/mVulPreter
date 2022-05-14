ZEND_API zend_op_array *compile_file(zend_file_handle *file_handle, int type TSRMLS_DC)
{
	zend_lex_state original_lex_state;
	zend_op_array *op_array = (zend_op_array *) emalloc(sizeof(zend_op_array));
	zend_op_array *original_active_op_array = CG(active_op_array);
	zend_op_array *retval=NULL;
	int compiler_result;
	zend_bool compilation_successful=0;
	znode retval_znode;
	zend_bool original_in_compilation = CG(in_compilation);

	retval_znode.op_type = IS_CONST;
	retval_znode.u.constant.type = IS_LONG;
	retval_znode.u.constant.value.lval = 1;
	Z_UNSET_ISREF(retval_znode.u.constant);
	Z_SET_REFCOUNT(retval_znode.u.constant, 1);

	zend_save_lexical_state(&original_lex_state TSRMLS_CC);

	retval = op_array; /* success oriented */

	if (open_file_for_scanning(file_handle TSRMLS_CC)==FAILURE) {
		if (type==ZEND_REQUIRE) {
			zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename TSRMLS_CC);
			zend_bailout();
		} else {
			zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename TSRMLS_CC);
		}
		compilation_successful=0;
	} else {
		init_op_array(op_array, ZEND_USER_FUNCTION, INITIAL_OP_ARRAY_SIZE TSRMLS_CC);
		CG(in_compilation) = 1;
		CG(active_op_array) = op_array;
		zend_stack_push(&CG(context_stack), (void *) &CG(context), sizeof(CG(context)));
		zend_init_compiler_context(TSRMLS_C);
 		compiler_result = zendparse(TSRMLS_C);
 		zend_do_return(&retval_znode, 0 TSRMLS_CC);
 		CG(in_compilation) = original_in_compilation;
		if (compiler_result==1) { /* parser error */
 			zend_bailout();
 		}
 		compilation_successful=1;
	}

	if (retval) {
		CG(active_op_array) = original_active_op_array;
		if (compilation_successful) {
			pass_two(op_array TSRMLS_CC);
			zend_release_labels(0 TSRMLS_CC);
		} else {
			efree(op_array);
			retval = NULL;
		}
	}
	zend_restore_lexical_state(&original_lex_state TSRMLS_CC);
	return retval;
}
