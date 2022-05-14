 static void php_mcrypt_module_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
 {
 	php_mcrypt *pm = (php_mcrypt *) rsrc->ptr;
	if (pm) {
 		mcrypt_generic_deinit(pm->td);
 		mcrypt_module_close(pm->td);
 		efree(pm);
		pm = NULL;
	}
}
/* }}} */
