static inline unsigned char unimap_bsearch(const uni_to_enc *table, unsigned code_key_a, size_t num)
{
	const uni_to_enc *l = table,
					 *h = &table[num-1],
					 *m;
	unsigned short code_key;

	/* we have no mappings outside the BMP */
	if (code_key_a > 0xFFFFU)
 		return 0;
 
 	code_key = (unsigned short) code_key_a;
 	while (l <= h) {
 		m = l + (h - l) / 2;
 		if (code_key < m->un_code_point)
			h = m - 1;
		else if (code_key > m->un_code_point)
			l = m + 1;
		else
			return m->cs_code;
	}
	return 0;
}
