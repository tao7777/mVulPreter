PHP_FUNCTION(pg_trace)
{
	char *z_filename, *mode = "w";
	int z_filename_len, mode_len;
	zval *pgsql_link = NULL;
	int id = -1, argc = ZEND_NUM_ARGS();
	PGconn *pgsql;
	FILE *fp = NULL;
        php_stream *stream;
        id = PGG(default_link);
 
       if (zend_parse_parameters(argc TSRMLS_CC, "s|sr", &z_filename, &z_filename_len, &mode, &mode_len, &pgsql_link) == FAILURE) {
                return;
        }
	if (argc < 3) {
		CHECK_DEFAULT_LINK(id);
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	stream = php_stream_open_wrapper(z_filename, mode, REPORT_ERRORS, NULL);

	if (!stream) {
		RETURN_FALSE;
	}

	if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_STDIO, (void**)&fp, REPORT_ERRORS))	{
		php_stream_close(stream);
		RETURN_FALSE;
	}
	php_stream_auto_cleanup(stream);
	PQtrace(pgsql, fp);
	RETURN_TRUE;
}
