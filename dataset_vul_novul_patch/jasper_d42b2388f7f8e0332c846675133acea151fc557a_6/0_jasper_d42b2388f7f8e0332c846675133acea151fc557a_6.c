jas_iccprof_t *jas_iccprof_createfrombuf(uchar *buf, int len)
jas_iccprof_t *jas_iccprof_createfrombuf(jas_uchar *buf, int len)
 {
 	jas_stream_t *in;
 	jas_iccprof_t *prof;
	if (!(in = jas_stream_memopen(JAS_CAST(char *, buf), len)))
		goto error;
	if (!(prof = jas_iccprof_load(in)))
		goto error;
	jas_stream_close(in);
	return prof;
error:
	if (in)
		jas_stream_close(in);
	return 0;
}
