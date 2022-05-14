static int jas_iccgetuint(jas_stream_t *in, int n, ulonglong *val)
static int jas_iccgetuint(jas_stream_t *in, int n, jas_ulonglong *val)
 {
 	int i;
 	int c;
	jas_ulonglong v;
 	v = 0;
 	for (i = n; i > 0; --i) {
 		if ((c = jas_stream_getc(in)) == EOF)
			return -1;
		v = (v << 8) | c;
	}
	*val = v;
	return 0;
}
