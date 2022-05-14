SPL_METHOD(SplFileInfo, setInfoClass)
{
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	zend_class_entry *ce = spl_ce_SplFileInfo;
 	zend_error_handling error_handling;
 	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling  TSRMLS_CC);
 
 	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|C", &ce) == SUCCESS) {
		intern->info_class = ce;
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
}
