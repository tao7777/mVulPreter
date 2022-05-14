PHPAPI php_stream *_php_stream_memory_open(int mode, char *buf, size_t length STREAMS_DC TSRMLS_DC)
{
	php_stream *stream;
	php_stream_memory_data *ms;
 
        if ((stream = php_stream_memory_create_rel(mode)) != NULL) {
                ms = (php_stream_memory_data*)stream->abstract;

                if (mode == TEMP_STREAM_READONLY || mode == TEMP_STREAM_TAKE_BUFFER) {
                        /* use the buffer directly */
                        ms->data = buf;
			ms->fsize = length;
		} else {
			if (length) {
				assert(buf != NULL);
				php_stream_write(stream, buf, length);
			}
		}
	}
	return stream;
}
