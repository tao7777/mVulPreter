 static int jas_iccgetuint64(jas_stream_t *in, jas_iccuint64_t *val)
 {
	jas_ulonglong tmp;
 	if (jas_iccgetuint(in, 8, &tmp))
 		return -1;
 	*val = tmp;
 	return 0;
 }
