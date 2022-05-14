 int pnm_validate(jas_stream_t *in)
 {
	jas_uchar buf[2];
 	int i;
 	int n;
 
	assert(JAS_STREAM_MAXPUTBACK >= 2);

	/* Read the first two characters that constitute the signature. */
	if ((n = jas_stream_read(in, buf, 2)) < 0) {
		return -1;
	}
	/* Put these characters back to the stream. */
	for (i = n - 1; i >= 0; --i) {
		if (jas_stream_ungetc(in, buf[i]) == EOF) {
			return -1;
		}
	}
	/* Did we read enough data? */
	if (n < 2) {
		return -1;
	}
	/* Is this the correct signature for a PNM file? */
	if (buf[0] == 'P' && isdigit(buf[1])) {
		return 0;
	}
	return -1;
}
