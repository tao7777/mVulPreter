static PHP_FUNCTION(readgzfile)
{
	char *filename;
	int filename_len;
	int flags = REPORT_ERRORS;
	php_stream *stream;
        int size;
        long use_include_path = 0;
 
       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &filename, &filename_len, &use_include_path) == FAILURE) {
                return;
        }
	if (use_include_path) {
		flags |= USE_PATH;
	}

	stream = php_stream_gzopen(NULL, filename, "rb", flags, NULL, NULL STREAMS_CC TSRMLS_CC);

	if (!stream) {
		RETURN_FALSE;
	}
	size = php_stream_passthru(stream);
	php_stream_close(stream);
	RETURN_LONG(size);
}
