PHP_FUNCTION(finfo_open)
{
	long options = MAGIC_NONE;
	char *file = NULL;
	int file_len = 0;
	struct php_fileinfo *finfo;
	FILEINFO_DECLARE_INIT_OBJECT(object)
	char resolved_path[MAXPATHLEN];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lp", &options, &file, &file_len) == FAILURE) {
		FILEINFO_DESTROY_OBJECT(object);
		RETURN_FALSE;
	}

	if (object) {
		struct finfo_object *finfo_obj = (struct finfo_object*)zend_object_store_get_object(object TSRMLS_CC);

		if (finfo_obj->ptr) {
			magic_close(finfo_obj->ptr->magic);
			efree(finfo_obj->ptr);
			finfo_obj->ptr = NULL;
		}
	}

	if (file_len == 0) {
		file = NULL;
	} else if (file && *file) { /* user specified file, perform open_basedir checks */

#if PHP_API_VERSION < 20100412
		if ((PG(safe_mode) && (!php_checkuid(file, NULL, CHECKUID_CHECK_FILE_AND_DIR))) || php_check_open_basedir(file TSRMLS_CC)) {
#else
		if (php_check_open_basedir(file TSRMLS_CC)) {
#endif
			FILEINFO_DESTROY_OBJECT(object);
			RETURN_FALSE;
		}
		if (!expand_filepath_with_mode(file, resolved_path, NULL, 0, CWD_EXPAND TSRMLS_CC)) {
			FILEINFO_DESTROY_OBJECT(object);
			RETURN_FALSE;
		}
		file = resolved_path;
	}

	finfo = emalloc(sizeof(struct php_fileinfo));

	finfo->options = options;
	finfo->magic = magic_open(options);

	if (finfo->magic == NULL) {
		efree(finfo);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid mode '%ld'.", options);
		FILEINFO_DESTROY_OBJECT(object);
		RETURN_FALSE;
	}

	if (magic_load(finfo->magic, file) == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to load magic database at '%s'.", file);
		magic_close(finfo->magic);
		efree(finfo);
		FILEINFO_DESTROY_OBJECT(object);
		RETURN_FALSE;
	}

	if (object) {
		FILEINFO_REGISTER_OBJECT(object, finfo);
	} else {
		ZEND_REGISTER_RESOURCE(return_value, finfo, le_fileinfo);
	}
}
/* }}} */

/* {{{ proto resource finfo_close(resource finfo)
   Close fileinfo resource. */
PHP_FUNCTION(finfo_close)
{
	struct php_fileinfo *finfo;
	zval *zfinfo;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zfinfo) == FAILURE) {
		RETURN_FALSE;
	}
	ZEND_FETCH_RESOURCE(finfo, struct php_fileinfo *, &zfinfo, -1, "file_info", le_fileinfo);

	zend_list_delete(Z_RESVAL_P(zfinfo));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool finfo_set_flags(resource finfo, int options)
   Set libmagic configuration options. */
PHP_FUNCTION(finfo_set_flags)
{
	long options;
	struct php_fileinfo *finfo;
	zval *zfinfo;
	FILEINFO_DECLARE_INIT_OBJECT(object)

	if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &options) == FAILURE) {
			RETURN_FALSE;
		}
		FILEINFO_FROM_OBJECT(finfo, object);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zfinfo, &options) == FAILURE) {
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(finfo, struct php_fileinfo *, &zfinfo, -1, "file_info", le_fileinfo);
	}

	FINFO_SET_OPTION(finfo->magic, options)
	finfo->options = options;

	RETURN_TRUE;
}
/* }}} */

#define FILEINFO_MODE_BUFFER 0
#define FILEINFO_MODE_STREAM 1
#define FILEINFO_MODE_FILE 2

static void _php_finfo_get_type(INTERNAL_FUNCTION_PARAMETERS, int mode, int mimetype_emu) /* {{{ */
{
	long options = 0;
	char *ret_val = NULL, *buffer = NULL;
	int buffer_len;
	struct php_fileinfo *finfo = NULL;
	zval *zfinfo, *zcontext = NULL;
	zval *what;
	char mime_directory[] = "directory";

	struct magic_set *magic = NULL;
	FILEINFO_DECLARE_INIT_OBJECT(object)

	if (mimetype_emu) {

		/* mime_content_type(..) emulation */
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &what) == FAILURE) {
			return;
		}

		switch (Z_TYPE_P(what)) {
			case IS_STRING:
				buffer = Z_STRVAL_P(what);
				buffer_len = Z_STRLEN_P(what);
				mode = FILEINFO_MODE_FILE;
				break;

			case IS_RESOURCE:
				mode = FILEINFO_MODE_STREAM;
				break;

			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can only process string or stream arguments");
				RETURN_FALSE;
		}

		magic = magic_open(MAGIC_MIME_TYPE);
		if (magic_load(magic, NULL) == -1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to load magic database.");
			goto common;
		}
	} else if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lr", &buffer, &buffer_len, &options, &zcontext) == FAILURE) {
			RETURN_FALSE;
		}
		FILEINFO_FROM_OBJECT(finfo, object);
		magic = finfo->magic;
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|lr", &zfinfo, &buffer, &buffer_len, &options, &zcontext) == FAILURE) {
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(finfo, struct php_fileinfo *, &zfinfo, -1, "file_info", le_fileinfo);
		magic = finfo->magic;
	}	

	/* Set options for the current file/buffer. */
	if (options) {
		FINFO_SET_OPTION(magic, options)
	}

	switch (mode) {
		case FILEINFO_MODE_BUFFER:
		{
			ret_val = (char *) magic_buffer(magic, buffer, buffer_len);
			break;
		}

		case FILEINFO_MODE_STREAM:
		{
				php_stream *stream;
				off_t streampos;

				php_stream_from_zval_no_verify(stream, &what);
				if (!stream) {
					goto common;
				}

				streampos = php_stream_tell(stream); /* remember stream position for restoration */
				php_stream_seek(stream, 0, SEEK_SET);

				ret_val = (char *) magic_stream(magic, stream);

				php_stream_seek(stream, streampos, SEEK_SET);
				break;
		}

		case FILEINFO_MODE_FILE:
		{
			/* determine if the file is a local file or remote URL */
			char *tmp2;
			php_stream_wrapper *wrap;
			php_stream_statbuf ssb;

			if (buffer == NULL || !*buffer) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty filename or path");
                                RETVAL_FALSE;
                                goto clean;
                        }
                       if (CHECK_NULL_PATH(buffer, buffer_len)) {
                               php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid path");
                               RETVAL_FALSE;
                               goto clean;
                       }
 
                        wrap = php_stream_locate_url_wrapper(buffer, &tmp2, 0 TSRMLS_CC);
				if (php_stream_stat_path_ex(buffer, 0, &ssb, context) == SUCCESS) {
					if (ssb.sb.st_mode & S_IFDIR) {
						ret_val = mime_directory;
						goto common;
					}
				}
#endif

#if PHP_API_VERSION < 20100412
				stream = php_stream_open_wrapper_ex(buffer, "rb", ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, context);
#else
				stream = php_stream_open_wrapper_ex(buffer, "rb", REPORT_ERRORS, NULL, context);
#endif

				if (!stream) {
					RETVAL_FALSE;
					goto clean;
				}

				if (php_stream_stat(stream, &ssb) == SUCCESS) {
					if (ssb.sb.st_mode & S_IFDIR) {
						ret_val = mime_directory;
					} else {
						ret_val = (char *)magic_stream(magic, stream);
					}
				}

				php_stream_close(stream);
			}
			break;
		}

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can only process string or stream arguments");
	}

common:
	if (ret_val) {
		RETVAL_STRING(ret_val, 1);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed identify data %d:%s", magic_errno(magic), magic_error(magic));
		RETVAL_FALSE;
	}

clean:
	if (mimetype_emu) {
		magic_close(magic);
	}

	/* Restore options */
	if (options) {
		FINFO_SET_OPTION(magic, finfo->options)
	}
	return;
}
