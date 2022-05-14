static int jas_iccputuint(jas_stream_t *out, int n, ulonglong val)
static int jas_iccputuint(jas_stream_t *out, int n, jas_ulonglong val)
 {
 	int i;
 	int c;
	for (i = n; i > 0; --i) {
		c = (val >> (8 * (i - 1))) & 0xff;
		if (jas_stream_putc(out, c) == EOF)
			return -1;
	}
 	return 0;
 }
