 SPL_METHOD(DirectoryIterator, rewind)
 {
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	intern->u.dir.index = 0;
	if (intern->u.dir.dirp) {
		php_stream_rewinddir(intern->u.dir.dirp);
	}
	spl_filesystem_dir_read(intern TSRMLS_CC);
}
