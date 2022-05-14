 SPL_METHOD(SplFileObject, ftell)
 {
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);	
 	long ret = php_stream_tell(intern->u.file.stream);
 
 	if (ret == -1) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(ret);
	}
} /* }}} */

/* {{{ proto int SplFileObject::fseek(int pos [, int whence = SEEK_SET])
