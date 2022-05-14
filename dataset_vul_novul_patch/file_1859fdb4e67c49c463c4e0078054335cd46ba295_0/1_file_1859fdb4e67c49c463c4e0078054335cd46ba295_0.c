 cdf_read_sector(const cdf_info_t *info, void *buf, size_t offs, size_t len,
     const cdf_header_t *h, cdf_secid_t id)
 {
	assert((size_t)CDF_SEC_SIZE(h) == len);
	return cdf_read(info, (off_t)CDF_SEC_POS(h, id),
	    ((char *)buf) + offs, len);
 }
