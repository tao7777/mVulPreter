hstore_recv(PG_FUNCTION_ARGS)
{
	int32		buflen;
	HStore	   *out;
	Pairs	   *pairs;
	int32		i;
	int32		pcount;
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);

	pcount = pq_getmsgint(buf, 4);

	if (pcount == 0)
	{
		out = hstorePairs(NULL, 0, 0);
 		PG_RETURN_POINTER(out);
 	}
 
	if (pcount < 0 || pcount > MaxAllocSize / sizeof(Pairs))
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
			  errmsg("number of pairs (%d) exceeds the maximum allowed (%d)",
					 pcount, (int) (MaxAllocSize / sizeof(Pairs)))));
 	pairs = palloc(pcount * sizeof(Pairs));
 
 	for (i = 0; i < pcount; ++i)
	{
		int			rawlen = pq_getmsgint(buf, 4);
		int			len;

		if (rawlen < 0)
			ereport(ERROR,
					(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
					 errmsg("null value not allowed for hstore key")));

		pairs[i].key = pq_getmsgtext(buf, rawlen, &len);
		pairs[i].keylen = hstoreCheckKeyLen(len);
		pairs[i].needfree = true;

		rawlen = pq_getmsgint(buf, 4);
		if (rawlen < 0)
		{
			pairs[i].val = NULL;
			pairs[i].vallen = 0;
			pairs[i].isnull = true;
		}
		else
		{
			pairs[i].val = pq_getmsgtext(buf, rawlen, &len);
			pairs[i].vallen = hstoreCheckValLen(len);
			pairs[i].isnull = false;
		}
	}

	pcount = hstoreUniquePairs(pairs, pcount, &buflen);

	out = hstorePairs(pairs, pcount, buflen);

	PG_RETURN_POINTER(out);
}
