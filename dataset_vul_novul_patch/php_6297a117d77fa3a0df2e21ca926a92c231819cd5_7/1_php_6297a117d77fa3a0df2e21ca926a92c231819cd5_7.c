 static int php_stream_temp_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC) /* {{{ */
 {
        php_stream_temp_data *ts = (php_stream_temp_data*)stream->abstract;
        switch(option) {
                case PHP_STREAM_OPTION_META_DATA_API:
                        if (ts->meta) {
				zend_hash_copy(Z_ARRVAL_P((zval*)ptrparam), Z_ARRVAL_P(ts->meta), (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval*));
			}
			return PHP_STREAM_OPTION_RETURN_OK;
		default:
			if (ts->innerstream) {
				return php_stream_set_option(ts->innerstream, option, value, ptrparam);
			}
			return PHP_STREAM_OPTION_RETURN_NOTIMPL;
	}
}
/* }}} */
