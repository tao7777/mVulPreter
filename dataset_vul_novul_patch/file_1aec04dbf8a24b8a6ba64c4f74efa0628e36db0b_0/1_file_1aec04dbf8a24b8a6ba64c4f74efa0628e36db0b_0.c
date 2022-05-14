 cdf_read_short_sector(const cdf_stream_t *sst, void *buf, size_t offs,
     size_t len, const cdf_header_t *h, cdf_secid_t id)
 {
	size_t ss = CDF_SEC_SIZE(h);
 	size_t pos = CDF_SHORT_SEC_POS(h, id);
 	assert(ss == len);
	if (pos > ss * sst->sst_len) {
 		DPRINTF(("Out of bounds read %" SIZE_T_FORMAT "u > %"
 		    SIZE_T_FORMAT "u\n",
		    pos, ss * sst->sst_len));
 		return -1;
 	}
 	(void)memcpy(((char *)buf) + offs,
	    ((const char *)sst->sst_tab) + pos, len);
	return len;
}
