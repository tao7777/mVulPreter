parse_tsquery(char *buf,
			  PushFunction pushval,
			  Datum opaque,
			  bool isplain)
{
	struct TSQueryParserStateData state;
	int			i;
	TSQuery		query;
	int			commonlen;
	QueryItem  *ptr;
	ListCell   *cell;

	/* init state */
	state.buffer = buf;
	state.buf = buf;
	state.state = (isplain) ? WAITSINGLEOPERAND : WAITFIRSTOPERAND;
	state.count = 0;
	state.polstr = NIL;

	/* init value parser's state */
	state.valstate = init_tsvector_parser(state.buffer, true, true);

	/* init list of operand */
	state.sumlen = 0;
	state.lenop = 64;
	state.curop = state.op = (char *) palloc(state.lenop);
	*(state.curop) = '\0';

	/* parse query & make polish notation (postfix, but in reverse order) */
	makepol(&state, pushval, opaque);

	close_tsvector_parser(state.valstate);

	if (list_length(state.polstr) == 0)
	{
		ereport(NOTICE,
				(errmsg("text-search query doesn't contain lexemes: \"%s\"",
						state.buffer)));
		query = (TSQuery) palloc(HDRSIZETQ);
		SET_VARSIZE(query, HDRSIZETQ);
		query->size = 0;
 		return query;
 	}
 
	if (TSQUERY_TOO_BIG(list_length(state.polstr), state.sumlen))
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
				 errmsg("tsquery is too large")));
 	commonlen = COMPUTESIZE(list_length(state.polstr), state.sumlen);

	/* Pack the QueryItems in the final TSQuery struct to return to caller */
 	query = (TSQuery) palloc0(commonlen);
 	SET_VARSIZE(query, commonlen);
 	query->size = list_length(state.polstr);
	ptr = GETQUERY(query);

	/* Copy QueryItems to TSQuery */
	i = 0;
	foreach(cell, state.polstr)
	{
		QueryItem  *item = (QueryItem *) lfirst(cell);

		switch (item->type)
		{
			case QI_VAL:
				memcpy(&ptr[i], item, sizeof(QueryOperand));
				break;
			case QI_VALSTOP:
				ptr[i].type = QI_VALSTOP;
				break;
			case QI_OPR:
				memcpy(&ptr[i], item, sizeof(QueryOperator));
				break;
			default:
				elog(ERROR, "unrecognized QueryItem type: %d", item->type);
		}
		i++;
	}

	/* Copy all the operand strings to TSQuery */
	memcpy((void *) GETOPERAND(query), (void *) state.op, state.sumlen);
	pfree(state.op);

	/* Set left operand pointers for every operator. */
	findoprnd(ptr, query->size);

	return query;
}
