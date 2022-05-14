SPL_METHOD(SplFileObject, setMaxLineLen)
{
	long max_len;

	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &max_len) == FAILURE) {
		return;
	}

	if (max_len < 0) {
 		zend_throw_exception_ex(spl_ce_DomainException, 0 TSRMLS_CC, "Maximum line length must be greater than or equal zero");
 		return;
 	}

 	intern->u.file.max_line_len = max_len;
 } /* }}} */
 
/* {{{ proto int SplFileObject::getMaxLineLen()
