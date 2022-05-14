cdf_read_short_sector_chain(const cdf_header_t *h,
     const cdf_sat_t *ssat, const cdf_stream_t *sst,
     cdf_secid_t sid, size_t len, cdf_stream_t *scn)
 {
	size_t ss = CDF_SHORT_SEC_SIZE(h), i, j;
 	scn->sst_len = cdf_count_chain(ssat, sid, CDF_SEC_SIZE(h));
 	scn->sst_dirlen = len;
 
	if (sst->sst_tab == NULL || scn->sst_len == (size_t)-1)
		return -1;

	scn->sst_tab = calloc(scn->sst_len, ss);
	if (scn->sst_tab == NULL)
		return -1;

	for (j = i = 0; sid >= 0; i++, j++) {
		if (j >= CDF_LOOP_LIMIT) {
			DPRINTF(("Read short sector chain loop limit"));
			errno = EFTYPE;
			goto out;
		}
		if (i >= scn->sst_len) {
			DPRINTF(("Out of bounds reading short sector chain "
			    "%" SIZE_T_FORMAT "u > %" SIZE_T_FORMAT "u\n",
			    i, scn->sst_len));
			errno = EFTYPE;
			goto out;
		}
		if (cdf_read_short_sector(sst, scn->sst_tab, i * ss, ss, h,
		    sid) != (ssize_t)ss) {
			DPRINTF(("Reading short sector chain %d", sid));
			goto out;
		}
		sid = CDF_TOLE4((uint32_t)ssat->sat_tab[sid]);
	}
	return 0;
out:
	free(scn->sst_tab);
	return -1;
}
