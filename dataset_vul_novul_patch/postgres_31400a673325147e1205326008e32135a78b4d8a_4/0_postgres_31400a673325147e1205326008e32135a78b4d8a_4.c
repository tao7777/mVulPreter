hstore_slice_to_hstore(PG_FUNCTION_ARGS)
{
	HStore	   *hs = PG_GETARG_HS(0);
	HEntry	   *entries = ARRPTR(hs);
	char	   *ptr = STRPTR(hs);
	ArrayType  *key_array = PG_GETARG_ARRAYTYPE_P(1);
	HStore	   *out;
	int			nkeys;
	Pairs	   *key_pairs = hstoreArrayToPairs(key_array, &nkeys);
	Pairs	   *out_pairs;
	int			bufsiz;
	int			lastidx = 0;
	int			i;
	int			out_count = 0;

	if (nkeys == 0)
	{
		out = hstorePairs(NULL, 0, 0);
 		PG_RETURN_POINTER(out);
 	}
 
	/* hstoreArrayToPairs() checked overflow */
 	out_pairs = palloc(sizeof(Pairs) * nkeys);
 	bufsiz = 0;
 
	/*
	 * we exploit the fact that the pairs list is already sorted into strictly
	 * increasing order to narrow the hstoreFindKey search; each search can
	 * start one entry past the previous "found" entry, or at the lower bound
	 * of the last search.
	 */

	for (i = 0; i < nkeys; ++i)
	{
		int			idx = hstoreFindKey(hs, &lastidx,
									  key_pairs[i].key, key_pairs[i].keylen);

		if (idx >= 0)
		{
			out_pairs[out_count].key = key_pairs[i].key;
			bufsiz += (out_pairs[out_count].keylen = key_pairs[i].keylen);
			out_pairs[out_count].val = HS_VAL(entries, ptr, idx);
			bufsiz += (out_pairs[out_count].vallen = HS_VALLEN(entries, idx));
			out_pairs[out_count].isnull = HS_VALISNULL(entries, idx);
			out_pairs[out_count].needfree = false;
			++out_count;
		}
	}

	/*
	 * we don't use uniquePairs here because we know that the pairs list is
	 * already sorted and uniq'ed.
	 */

	out = hstorePairs(out_pairs, out_count, bufsiz);

	PG_RETURN_POINTER(out);
}
