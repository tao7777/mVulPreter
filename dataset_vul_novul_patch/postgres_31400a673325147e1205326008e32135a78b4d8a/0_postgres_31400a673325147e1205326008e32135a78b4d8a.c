hstore_from_array(PG_FUNCTION_ARGS)
{
	ArrayType  *in_array = PG_GETARG_ARRAYTYPE_P(0);
	int			ndims = ARR_NDIM(in_array);
	int			count;
	int32		buflen;
	HStore	   *out;
	Pairs	   *pairs;
	Datum	   *in_datums;
	bool	   *in_nulls;
	int			in_count;
	int			i;

	Assert(ARR_ELEMTYPE(in_array) == TEXTOID);

	switch (ndims)
	{
		case 0:
			out = hstorePairs(NULL, 0, 0);
			PG_RETURN_POINTER(out);

		case 1:
			if ((ARR_DIMS(in_array)[0]) % 2)
				ereport(ERROR,
						(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
						 errmsg("array must have even number of elements")));
			break;

		case 2:
			if ((ARR_DIMS(in_array)[1]) != 2)
				ereport(ERROR,
						(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
						 errmsg("array must have two columns")));
			break;

		default:
			ereport(ERROR,
					(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
					 errmsg("wrong number of array subscripts")));
	}

	deconstruct_array(in_array,
					  TEXTOID, -1, false, 'i',
					  &in_datums, &in_nulls, &in_count);
 
 	count = in_count / 2;
 
	/* see discussion in hstoreArrayToPairs() */
	if (count > MaxAllocSize / sizeof(Pairs))
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
			  errmsg("number of pairs (%d) exceeds the maximum allowed (%d)",
					 count, (int) (MaxAllocSize / sizeof(Pairs)))));

 	pairs = palloc(count * sizeof(Pairs));
 
 	for (i = 0; i < count; ++i)
	{
		if (in_nulls[i * 2])
			ereport(ERROR,
					(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
					 errmsg("null value not allowed for hstore key")));

		if (in_nulls[i * 2 + 1])
		{
			pairs[i].key = VARDATA_ANY(in_datums[i * 2]);
			pairs[i].val = NULL;
			pairs[i].keylen = hstoreCheckKeyLen(VARSIZE_ANY_EXHDR(in_datums[i * 2]));
			pairs[i].vallen = 4;
			pairs[i].isnull = true;
			pairs[i].needfree = false;
		}
		else
		{
			pairs[i].key = VARDATA_ANY(in_datums[i * 2]);
			pairs[i].val = VARDATA_ANY(in_datums[i * 2 + 1]);
			pairs[i].keylen = hstoreCheckKeyLen(VARSIZE_ANY_EXHDR(in_datums[i * 2]));
			pairs[i].vallen = hstoreCheckValLen(VARSIZE_ANY_EXHDR(in_datums[i * 2 + 1]));
			pairs[i].isnull = false;
			pairs[i].needfree = false;
		}
	}

	count = hstoreUniquePairs(pairs, count, &buflen);

	out = hstorePairs(pairs, count, buflen);

	PG_RETURN_POINTER(out);
}
