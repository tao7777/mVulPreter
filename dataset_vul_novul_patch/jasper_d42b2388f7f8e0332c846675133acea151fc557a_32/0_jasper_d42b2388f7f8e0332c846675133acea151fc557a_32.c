int bmp_validate(jas_stream_t *in)
 {
 	int n;
 	int i;
	jas_uchar buf[2];
 
 	assert(JAS_STREAM_MAXPUTBACK >= 2);
 
	/* Read the first two characters that constitute the signature. */
	if ((n = jas_stream_read(in, (char *) buf, 2)) < 0) {
		return -1;
	}
	/* Put the characters read back onto the stream. */
	for (i = n - 1; i >= 0; --i) {
		if (jas_stream_ungetc(in, buf[i]) == EOF) {
			return -1;
		}
	}
	/* Did we read enough characters? */
	if (n < 2) {
		return -1;
	}
	/* Is the signature correct for the BMP format? */
	if (buf[0] == (BMP_MAGIC & 0xff) && buf[1] == (BMP_MAGIC >> 8)) {
		return 0;
	}
	return -1;
}
