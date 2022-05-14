hstoreArrayToPairs(ArrayType *a, int *npairs)
{
	Datum	   *key_datums;
	bool	   *key_nulls;
	int			key_count;
	Pairs	   *key_pairs;
	int			bufsiz;
	int			i,
				j;

	deconstruct_array(a,
					  TEXTOID, -1, false, 'i',
					  &key_datums, &key_nulls, &key_count);

	if (key_count == 0)
	{
		*npairs = 0;
 		return NULL;
 	}
 
	/*
	 * A text array uses at least eight bytes per element, so any overflow in
	 * "key_count * sizeof(Pairs)" is small enough for palloc() to catch.
	 * However, credible improvements to the array format could invalidate
	 * that assumption.  Therefore, use an explicit check rather than relying
	 * on palloc() to complain.
	 */
	if (key_count > MaxAllocSize / sizeof(Pairs))
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
			  errmsg("number of pairs (%d) exceeds the maximum allowed (%d)",
					 key_count, (int) (MaxAllocSize / sizeof(Pairs)))));

 	key_pairs = palloc(sizeof(Pairs) * key_count);
 
 	for (i = 0, j = 0; i < key_count; i++)
	{
		if (!key_nulls[i])
		{
			key_pairs[j].key = VARDATA(key_datums[i]);
			key_pairs[j].keylen = VARSIZE(key_datums[i]) - VARHDRSZ;
			key_pairs[j].val = NULL;
			key_pairs[j].vallen = 0;
			key_pairs[j].needfree = 0;
			key_pairs[j].isnull = 1;
			j++;
		}
	}

	*npairs = hstoreUniquePairs(key_pairs, j, &bufsiz);

	return key_pairs;
}
