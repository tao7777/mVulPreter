 int ras_validate(jas_stream_t *in)
 {
	jas_uchar buf[RAS_MAGICLEN];
 	int i;
 	int n;
 	uint_fast32_t magic;

	assert(JAS_STREAM_MAXPUTBACK >= RAS_MAGICLEN);

	/* Read the validation data (i.e., the data used for detecting
	  the format). */
	if ((n = jas_stream_read(in, buf, RAS_MAGICLEN)) < 0) {
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
	if (n < RAS_MAGICLEN) {
		return -1;
	}

	magic = (JAS_CAST(uint_fast32_t, buf[0]) << 24) |
	  (JAS_CAST(uint_fast32_t, buf[1]) << 16) |
	  (JAS_CAST(uint_fast32_t, buf[2]) << 8) |
	  buf[3];

	/* Is the signature correct for the Sun Rasterfile format? */
	if (magic != RAS_MAGIC) {
		return -1;
	}
	return 0;
}
