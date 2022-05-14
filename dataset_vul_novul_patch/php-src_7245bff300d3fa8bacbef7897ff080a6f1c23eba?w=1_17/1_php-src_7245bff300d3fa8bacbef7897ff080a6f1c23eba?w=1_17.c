 SPL_METHOD(FilesystemIterator, getFlags)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	RETURN_LONG(intern->flags & (SPL_FILE_DIR_KEY_MODE_MASK | SPL_FILE_DIR_CURRENT_MODE_MASK | SPL_FILE_DIR_OTHERS_MASK));
} /* }}} */

/* {{{ proto void FilesystemIterator::setFlags(long $flags)
