 static int jas_iccgetuint32(jas_stream_t *in, jas_iccuint32_t *val)
 {
	jas_ulonglong tmp;
 	if (jas_iccgetuint(in, 4, &tmp))
 		return -1;
 	*val = tmp;
	return 0;
}
