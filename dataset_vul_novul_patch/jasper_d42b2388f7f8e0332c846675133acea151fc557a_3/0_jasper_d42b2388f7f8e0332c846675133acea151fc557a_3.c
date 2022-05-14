 static int jas_iccgetuint16(jas_stream_t *in, jas_iccuint16_t *val)
 {
	jas_ulonglong tmp;
 	if (jas_iccgetuint(in, 2, &tmp))
 		return -1;
 	*val = tmp;
	return 0;
}
