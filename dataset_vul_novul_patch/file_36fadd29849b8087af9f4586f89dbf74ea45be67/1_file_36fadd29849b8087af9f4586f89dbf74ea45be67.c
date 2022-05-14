cdf_check_stream_offset(const cdf_stream_t *sst, const cdf_header_t *h,
    const void *p, size_t tail, int line)
 {
 	const char *b = (const char *)sst->sst_tab;
 	const char *e = ((const char *)p) + tail;
 	(void)&line;
	if (e >= b && (size_t)(e - b) <= CDF_SEC_SIZE(h) * sst->sst_len)
 		return 0;
 	DPRINTF(("%d: offset begin %p < end %p || %" SIZE_T_FORMAT "u"
 	    " > %" SIZE_T_FORMAT "u [%" SIZE_T_FORMAT "u %"
 	    SIZE_T_FORMAT "u]\n", line, b, e, (size_t)(e - b),
	    CDF_SEC_SIZE(h) * sst->sst_len, CDF_SEC_SIZE(h), sst->sst_len));
 	errno = EFTYPE;
 	return -1;
 }
