static int php_stream_temp_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	php_stream_temp_data *ts = (php_stream_temp_data*)stream->abstract;
	int ret;

	assert(ts != NULL);

	if (ts->innerstream) {
		ret = php_stream_free_enclosed(ts->innerstream, PHP_STREAM_FREE_CLOSE | (close_handle ? 0 : PHP_STREAM_FREE_PRESERVE_HANDLE));
        } else {
                ret = 0;
        }
        if (ts->meta) {
                zval_ptr_dtor(&ts->meta);
        }

	efree(ts);

	return ret;
}
