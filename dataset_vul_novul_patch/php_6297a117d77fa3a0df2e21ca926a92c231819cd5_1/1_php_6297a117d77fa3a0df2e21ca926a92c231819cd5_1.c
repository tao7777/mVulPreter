static int php_stream_memory_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC) /* {{{ */
 {
        php_stream_memory_data *ms = (php_stream_memory_data*)stream->abstract;
        size_t newsize;
        switch(option) {
                case PHP_STREAM_OPTION_TRUNCATE_API:
                        switch (value) {
				case PHP_STREAM_TRUNCATE_SUPPORTED:
					return PHP_STREAM_OPTION_RETURN_OK;

				case PHP_STREAM_TRUNCATE_SET_SIZE:
					if (ms->mode & TEMP_STREAM_READONLY) {
						return PHP_STREAM_OPTION_RETURN_ERR;
					}
					newsize = *(size_t*)ptrparam;
					if (newsize <= ms->fsize) {
						if (newsize < ms->fpos) {
							ms->fpos = newsize;
						}
					} else {
						ms->data = erealloc(ms->data, newsize);
						memset(ms->data+ms->fsize, 0, newsize - ms->fsize);
						ms->fsize = newsize;
					}
					ms->fsize = newsize;
					return PHP_STREAM_OPTION_RETURN_OK;
			}
		default:
			return PHP_STREAM_OPTION_RETURN_NOTIMPL;
        }
 }
 /* }}} */
