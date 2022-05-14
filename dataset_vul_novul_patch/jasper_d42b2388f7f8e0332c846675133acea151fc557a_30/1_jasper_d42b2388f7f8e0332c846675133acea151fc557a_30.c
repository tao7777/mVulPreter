static void jas_stream_initbuf(jas_stream_t *stream, int bufmode, char *buf,
  int bufsize)
{
	/* If this function is being called, the buffer should not have been
	  initialized yet. */
	assert(!stream->bufbase_);

	if (bufmode != JAS_STREAM_UNBUF) {
		/* The full- or line-buffered mode is being employed. */
		if (!buf) {
			/* The caller has not specified a buffer to employ, so allocate
			  one. */
			if ((stream->bufbase_ = jas_malloc(JAS_STREAM_BUFSIZE +
			  JAS_STREAM_MAXPUTBACK))) {
				stream->bufmode_ |= JAS_STREAM_FREEBUF;
				stream->bufsize_ = JAS_STREAM_BUFSIZE;
			} else {
				/* The buffer allocation has failed.  Resort to unbuffered
				  operation. */
				stream->bufbase_ = stream->tinybuf_;
				stream->bufsize_ = 1;
			}
		} else {
			/* The caller has specified a buffer to employ. */
 			/* The buffer must be large enough to accommodate maximum
 			  putback. */
 			assert(bufsize > JAS_STREAM_MAXPUTBACK);
			stream->bufbase_ = JAS_CAST(uchar *, buf);
 			stream->bufsize_ = bufsize - JAS_STREAM_MAXPUTBACK;
 		}
 	} else {
		/* The unbuffered mode is being employed. */
		/* A buffer should not have been supplied by the caller. */
		assert(!buf);
		/* Use a trivial one-character buffer. */
		stream->bufbase_ = stream->tinybuf_;
		stream->bufsize_ = 1;
	}
	stream->bufstart_ = &stream->bufbase_[JAS_STREAM_MAXPUTBACK];
	stream->ptr_ = stream->bufstart_;
	stream->cnt_ = 0;
	stream->bufmode_ |= bufmode & JAS_STREAM_BUFMODEMASK;
}
