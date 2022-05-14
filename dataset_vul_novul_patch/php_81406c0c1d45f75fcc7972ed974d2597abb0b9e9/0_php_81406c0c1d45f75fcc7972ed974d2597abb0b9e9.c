php_stream *php_stream_zip_open(char *filename, char *path, char *mode STREAMS_DC TSRMLS_DC)
{
	struct zip_file *zf = NULL;
	int err = 0;

	php_stream *stream = NULL;
	struct php_zip_stream_data_t *self;
	struct zip *stream_za;

	if (strncmp(mode,"r", strlen("r")) != 0) {
		return NULL;
	}

	if (filename) {
		if (ZIP_OPENBASEDIR_CHECKPATH(filename)) {
			return NULL;
		}

		/* duplicate to make the stream za independent (esp. for MSHUTDOWN) */
		stream_za = zip_open(filename, ZIP_CREATE, &err);
		if (!stream_za) {
			return NULL;
		}

		zf = zip_fopen(stream_za, path, 0);
		if (zf) {
                        self = emalloc(sizeof(*self));
 
                        self->za = stream_za;
                       self->zf = zf;
                        self->stream = NULL;
                        self->cursor = 0;
                        stream = php_stream_alloc(&php_stream_zipio_ops, self, NULL, mode);
			stream->orig_path = estrdup(path);
		} else {
			zip_close(stream_za);
		}
	}

	if (!stream) {
		return NULL;
	} else {
		return stream;
	}

}
