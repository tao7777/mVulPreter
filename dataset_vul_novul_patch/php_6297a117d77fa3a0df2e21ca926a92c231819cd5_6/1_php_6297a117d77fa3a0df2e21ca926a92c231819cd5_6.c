static int php_stream_temp_seek(php_stream *stream, off_t offset, int whence, off_t *newoffs TSRMLS_DC)
{
	php_stream_temp_data *ts = (php_stream_temp_data*)stream->abstract;
	int ret;

	assert(ts != NULL);

	if (!ts->innerstream) {
		*newoffs = -1;
		return -1;
	}
        ret = php_stream_seek(ts->innerstream, offset, whence);
        *newoffs = php_stream_tell(ts->innerstream);
        stream->eof = ts->innerstream->eof;
        return ret;
 }
