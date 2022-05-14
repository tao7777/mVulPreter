PHPAPI php_stream *_php_stream_memory_create(int mode STREAMS_DC TSRMLS_DC)
{
	php_stream_memory_data *self;
	php_stream *stream;

	self = emalloc(sizeof(*self));
	self->data = NULL;
	self->fpos = 0;
        self->fsize = 0;
        self->smax = ~0u;
        self->mode = mode;

        stream = php_stream_alloc_rel(&php_stream_memory_ops, self, 0, mode & TEMP_STREAM_READONLY ? "rb" : "w+b");
        stream->flags |= PHP_STREAM_FLAG_NO_BUFFER;
        return stream;
}
