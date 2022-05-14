 cdf_count_chain(const cdf_sat_t *sat, cdf_secid_t sid, size_t size)
 {
 	size_t i, j;
	cdf_secid_t maxsector = (cdf_secid_t)(sat->sat_len * size);
 
 	DPRINTF(("Chain:"));
 	for (j = i = 0; sid >= 0; i++, j++) {
		DPRINTF((" %d", sid));
		if (j >= CDF_LOOP_LIMIT) {
			DPRINTF(("Counting chain loop limit"));
 			errno = EFTYPE;
 			return (size_t)-1;
 		}
		if (sid > maxsector) {
			DPRINTF(("Sector %d > %d\n", sid, maxsector));
 			errno = EFTYPE;
 			return (size_t)-1;
 		}
		sid = CDF_TOLE4((uint32_t)sat->sat_tab[sid]);
	}
	if (i == 0) {
		DPRINTF((" none, sid: %d\n", sid));
		return (size_t)-1;

	}
	DPRINTF(("\n"));
	return i;
}
