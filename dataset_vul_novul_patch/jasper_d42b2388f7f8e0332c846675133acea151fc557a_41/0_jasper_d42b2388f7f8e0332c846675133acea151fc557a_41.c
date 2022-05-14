 int jpg_validate(jas_stream_t *in)
 {
	jas_uchar buf[JPG_MAGICLEN];
 	int i;
 	int n;
 
	assert(JAS_STREAM_MAXPUTBACK >= JPG_MAGICLEN);

	/* Read the validation data (i.e., the data used for detecting
	  the format). */
	if ((n = jas_stream_read(in, buf, JPG_MAGICLEN)) < 0) {
		return -1;
	}

	/* Put the validation data back onto the stream, so that the
	  stream position will not be changed. */
	for (i = n - 1; i >= 0; --i) {
		if (jas_stream_ungetc(in, buf[i]) == EOF) {
			return -1;
		}
	}

	/* Did we read enough data? */
	if (n < JPG_MAGICLEN) {
		return -1;
	}

	/* Does this look like JPEG? */
	if (buf[0] != (JPG_MAGIC >> 8) || buf[1] != (JPG_MAGIC & 0xff)) {
		return -1;
	}

	return 0;
}
