 cdf_read_short_sector(const cdf_stream_t *sst, void *buf, size_t offs,
     size_t len, const cdf_header_t *h, cdf_secid_t id)
 {
	size_t ss = CDF_SHORT_SEC_SIZE(h);
	size_t pos = CDF_SHORT_SEC_POS(h, id);
	assert(ss == len);
	if (sst->sst_len < (size_t)id) {
		DPRINTF(("bad sector id %d > %d\n", id, sst->sst_len));
		return -1;
	}
 	(void)memcpy(((char *)buf) + offs,
	    ((const char *)sst->sst_tab) + pos, len);
 	return len;
 }
