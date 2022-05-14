 int pgx_validate(jas_stream_t *in)
 {
	jas_uchar buf[PGX_MAGICLEN];
 	uint_fast32_t magic;
 	int i;
 	int n;

	assert(JAS_STREAM_MAXPUTBACK >= PGX_MAGICLEN);

	/* Read the validation data (i.e., the data used for detecting
	  the format). */
	if ((n = jas_stream_read(in, buf, PGX_MAGICLEN)) < 0) {
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
	if (n < PGX_MAGICLEN) {
		return -1;
	}

	/* Compute the signature value. */
	magic = (buf[0] << 8) | buf[1];

	/* Ensure that the signature is correct for this format. */
	if (magic != PGX_MAGIC) {
		return -1;
	}

	return 0;
}
