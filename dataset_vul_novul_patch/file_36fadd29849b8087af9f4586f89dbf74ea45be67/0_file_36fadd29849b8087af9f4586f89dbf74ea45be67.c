cdf_check_stream_offset(const cdf_stream_t *sst, const cdf_header_t *h,
    const void *p, size_t tail, int line)
 {
 	const char *b = (const char *)sst->sst_tab;
 	const char *e = ((const char *)p) + tail;
	size_t ss = sst->sst_dirlen < h->h_min_size_standard_stream ?
	    CDF_SHORT_SEC_SIZE(h) : CDF_SEC_SIZE(h);
 	(void)&line;
	if (e >= b && (size_t)(e - b) <= ss * sst->sst_len)
 		return 0;
 	DPRINTF(("%d: offset begin %p < end %p || %" SIZE_T_FORMAT "u"
 	    " > %" SIZE_T_FORMAT "u [%" SIZE_T_FORMAT "u %"
 	    SIZE_T_FORMAT "u]\n", line, b, e, (size_t)(e - b),
	    ss * sst->sst_len, ss, sst->sst_len));
 	errno = EFTYPE;
 	return -1;
 }
