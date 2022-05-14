 cdf_read_short_sector(const cdf_stream_t *sst, void *buf, size_t offs,
     size_t len, const cdf_header_t *h, cdf_secid_t id)
 {
	assert((size_t)CDF_SHORT_SEC_SIZE(h) == len);
 	(void)memcpy(((char *)buf) + offs,
	    ((const char *)sst->sst_tab) + CDF_SHORT_SEC_POS(h, id), len);
 	return len;
 }
