 static int jas_iccgetsint32(jas_stream_t *in, jas_iccsint32_t *val)
 {
	jas_ulonglong tmp;
 	if (jas_iccgetuint(in, 4, &tmp))
 		return -1;
	*val = (tmp & 0x80000000) ? (-JAS_CAST(jas_longlong, (((~tmp) &
	  0x7fffffff) + 1))) : JAS_CAST(jas_longlong, tmp);
 	return 0;
 }
