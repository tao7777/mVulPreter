static size_t php_stream_temp_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_stream_temp_data *ts = (php_stream_temp_data*)stream->abstract;
	size_t got;

	assert(ts != NULL);

        if (!ts->innerstream) {
                return -1;
        }
        got = php_stream_read(ts->innerstream, buf, count);
        stream->eof = ts->innerstream->eof;
        return got;
 }
