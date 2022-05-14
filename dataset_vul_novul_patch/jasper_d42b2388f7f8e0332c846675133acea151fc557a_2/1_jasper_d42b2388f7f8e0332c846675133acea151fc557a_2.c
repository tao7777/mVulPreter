static int jas_iccgetuint(jas_stream_t *in, int n, ulonglong *val)
 {
 	int i;
 	int c;
	ulonglong v;
 	v = 0;
 	for (i = n; i > 0; --i) {
 		if ((c = jas_stream_getc(in)) == EOF)
			return -1;
		v = (v << 8) | c;
	}
	*val = v;
	return 0;
}
