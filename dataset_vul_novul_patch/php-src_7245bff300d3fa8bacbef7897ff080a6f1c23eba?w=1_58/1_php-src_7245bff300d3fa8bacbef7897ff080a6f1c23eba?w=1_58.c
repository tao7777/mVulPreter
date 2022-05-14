 zend_function *spl_filesystem_object_get_method_check(zval **object_ptr, char *method, int method_len, const struct _zend_literal *key TSRMLS_DC) /* {{{ */
 {
 	spl_filesystem_object *fsobj = zend_object_store_get_object(*object_ptr TSRMLS_CC);
 	if (fsobj->u.dir.entry.d_name[0] == '\0' && fsobj->orig_path == NULL) {
 		method = "_bad_state_ex";
 		method_len = sizeof("_bad_state_ex") - 1;
 		key = NULL;
 	}
 	return zend_get_std_object_handlers()->get_method(object_ptr, method, method_len, key TSRMLS_CC);
 }
 /* }}} */
