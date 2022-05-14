 SPL_METHOD(GlobIterator, count)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	if (php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
		RETURN_LONG(php_glob_stream_get_count(intern->u.dir.dirp, NULL));
	} else {
		/* should not happen */
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "GlobIterator lost glob state");
	}
}
