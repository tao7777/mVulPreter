php_stream *php_stream_zip_opener(php_stream_wrapper *wrapper,
											char *path,
											char *mode,
											int options,
                                                                                        char **opened_path,
                                                                                        php_stream_context *context STREAMS_DC TSRMLS_DC)
 {
       size_t path_len;
 
        char *file_basename;
        size_t file_basename_len;
	char file_dirname[MAXPATHLEN];

        struct zip *za;
        struct zip_file *zf = NULL;
        char *fragment;
       size_t fragment_len;
        int err;
 
        php_stream *stream = NULL;
	struct php_zip_stream_data_t *self;

	fragment = strchr(path, '#');
	if (!fragment) {
		return NULL;
	}

	if (strncasecmp("zip://", path, 6) == 0) {
		path += 6;
	}

	fragment_len = strlen(fragment);

	if (fragment_len < 1) {
		return NULL;
	}
	path_len = strlen(path);
	if (path_len >= MAXPATHLEN || mode[0] != 'r') {
		return NULL;
	}

	memcpy(file_dirname, path, path_len - fragment_len);
	file_dirname[path_len - fragment_len] = '\0';

	php_basename(path, path_len - fragment_len, NULL, 0, &file_basename, &file_basename_len TSRMLS_CC);
	fragment++;

	if (ZIP_OPENBASEDIR_CHECKPATH(file_dirname)) {
		efree(file_basename);
		return NULL;
	}

	za = zip_open(file_dirname, ZIP_CREATE, &err);
	if (za) {
		zf = zip_fopen(za, fragment, 0);
		if (zf) {
                        self = emalloc(sizeof(*self));
 
                        self->za = za;
                       self->zf = zf;
                        self->stream = NULL;
                        self->cursor = 0;
                        stream = php_stream_alloc(&php_stream_zipio_ops, self, NULL, mode);

			if (opened_path) {
				*opened_path = estrdup(path);
			}
		} else {
			zip_close(za);
		}
	}

	efree(file_basename);

	if (!stream) {
		return NULL;
	} else {
		return stream;
	}
}
