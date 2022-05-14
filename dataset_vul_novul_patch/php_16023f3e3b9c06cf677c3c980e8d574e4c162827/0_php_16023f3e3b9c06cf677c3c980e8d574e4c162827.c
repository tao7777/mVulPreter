PHP_FUNCTION(openssl_random_pseudo_bytes)
{
        long buffer_length;
        unsigned char *buffer = NULL;
        zval *zstrong_result_returned = NULL;
 
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|z", &buffer_length, &zstrong_result_returned) == FAILURE) {
                return;
		return;
	}

	if (buffer_length <= 0) {
		RETURN_FALSE;
	}

	if (zstrong_result_returned) {
		zval_dtor(zstrong_result_returned);
		ZVAL_BOOL(zstrong_result_returned, 0);
	}
        buffer = emalloc(buffer_length + 1);
 
 #ifdef PHP_WIN32
        /* random/urandom equivalent on Windows */
        if (php_win32_get_random_bytes(buffer, (size_t) buffer_length) == FAILURE) {
                efree(buffer);
	if (php_win32_get_random_bytes(buffer, (size_t) buffer_length) == FAILURE) {
		efree(buffer);
		if (zstrong_result_returned) {
                RETURN_FALSE;
        }
 #else
       if (RAND_bytes(buffer, buffer_length) <= 0) {
                efree(buffer);
                if (zstrong_result_returned) {
                        ZVAL_BOOL(zstrong_result_returned, 0);
		if (zstrong_result_returned) {
			ZVAL_BOOL(zstrong_result_returned, 0);
		}
		RETURN_FALSE;
	}
#endif
        RETVAL_STRINGL((char *)buffer, buffer_length, 0);
 
        if (zstrong_result_returned) {
               ZVAL_BOOL(zstrong_result_returned, 1);
        }
 }
 /* }}} */
