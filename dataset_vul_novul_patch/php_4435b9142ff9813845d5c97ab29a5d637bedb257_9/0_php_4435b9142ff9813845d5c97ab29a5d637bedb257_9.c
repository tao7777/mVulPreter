static PHP_FUNCTION(gzopen)
{
	char *filename;
	char *mode;
	int filename_len, mode_len;
	int flags = REPORT_ERRORS;
        php_stream *stream;
        long use_include_path = 0;
 
       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ps|l", &filename, &filename_len, &mode, &mode_len, &use_include_path) == FAILURE) {
                return;
        }
 
	if (use_include_path) {
		flags |= USE_PATH;
	}

	stream = php_stream_gzopen(NULL, filename, mode, flags, NULL, NULL STREAMS_CC TSRMLS_CC);

	if (!stream) {
		RETURN_FALSE;
	}
	php_stream_to_zval(stream, return_value);
}
