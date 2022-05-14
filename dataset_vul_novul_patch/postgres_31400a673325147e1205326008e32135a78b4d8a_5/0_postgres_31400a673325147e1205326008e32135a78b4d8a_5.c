bqarr_in(PG_FUNCTION_ARGS)
{
	char	   *buf = (char *) PG_GETARG_POINTER(0);
	WORKSTATE	state;
	int32		i;
	QUERYTYPE  *query;
	int32		commonlen;
	ITEM	   *ptr;
	NODE	   *tmp;
	int32		pos = 0;

#ifdef BS_DEBUG
	StringInfoData pbuf;
#endif

	state.buf = buf;
	state.state = WAITOPERAND;
	state.count = 0;
	state.num = 0;
	state.str = NULL;

	/* make polish notation (postfix, but in reverse order) */
	makepol(&state);
	if (!state.num)
		ereport(ERROR,
 				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
 				 errmsg("empty query")));
 
	if (state.num > QUERYTYPEMAXITEMS)
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
		errmsg("number of query items (%d) exceeds the maximum allowed (%d)",
			   state.num, (int) QUERYTYPEMAXITEMS)));
 	commonlen = COMPUTESIZE(state.num);

 	query = (QUERYTYPE *) palloc(commonlen);
 	SET_VARSIZE(query, commonlen);
 	query->size = state.num;
	ptr = GETQUERY(query);

	for (i = state.num - 1; i >= 0; i--)
	{
		ptr[i].type = state.str->type;
		ptr[i].val = state.str->val;
		tmp = state.str->next;
		pfree(state.str);
		state.str = tmp;
	}

	pos = query->size - 1;
	findoprnd(ptr, &pos);
#ifdef BS_DEBUG
	initStringInfo(&pbuf);
	for (i = 0; i < query->size; i++)
	{
		if (ptr[i].type == OPR)
			appendStringInfo(&pbuf, "%c(%d) ", ptr[i].val, ptr[i].left);
		else
			appendStringInfo(&pbuf, "%d ", ptr[i].val);
	}
	elog(DEBUG3, "POR: %s", pbuf.data);
	pfree(pbuf.data);
#endif

	PG_RETURN_POINTER(query);
}
