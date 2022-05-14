queryin(char *buf)
{
	QPRS_STATE	state;
	int32		i;
	ltxtquery  *query;
	int32		commonlen;
	ITEM	   *ptr;
	NODE	   *tmp;
	int32		pos = 0;

#ifdef BS_DEBUG
	char		pbuf[16384],
			   *cur;
#endif

	/* init state */
	state.buf = buf;
	state.state = WAITOPERAND;
	state.count = 0;
	state.num = 0;
	state.str = NULL;

	/* init list of operand */
	state.sumlen = 0;
	state.lenop = 64;
	state.curop = state.op = (char *) palloc(state.lenop);
	*(state.curop) = '\0';

	/* parse query & make polish notation (postfix, but in reverse order) */
	makepol(&state);
	if (!state.num)
		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
 				 errmsg("syntax error"),
 				 errdetail("Empty query.")));
 
	if (LTXTQUERY_TOO_BIG(state.num, state.sumlen))
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
				 errmsg("ltxtquery is too large")));
 	commonlen = COMPUTESIZE(state.num, state.sumlen);

 	query = (ltxtquery *) palloc(commonlen);
 	SET_VARSIZE(query, commonlen);
 	query->size = state.num;
	ptr = GETQUERY(query);

	/* set item in polish notation */
	for (i = 0; i < state.num; i++)
	{
		ptr[i].type = state.str->type;
		ptr[i].val = state.str->val;
		ptr[i].distance = state.str->distance;
		ptr[i].length = state.str->length;
		ptr[i].flag = state.str->flag;
		tmp = state.str->next;
		pfree(state.str);
		state.str = tmp;
	}

	/* set user friendly-operand view */
	memcpy((void *) GETOPERAND(query), (void *) state.op, state.sumlen);
	pfree(state.op);

	/* set left operand's position for every operator */
	pos = 0;
	findoprnd(ptr, &pos);

	return query;
}
