static int jas_iccputsint(jas_stream_t *out, int n, longlong val)
 {
	ulonglong tmp;
 	tmp = (val < 0) ? (abort(), 0) : val;
 	return jas_iccputuint(out, n, tmp);
 }
