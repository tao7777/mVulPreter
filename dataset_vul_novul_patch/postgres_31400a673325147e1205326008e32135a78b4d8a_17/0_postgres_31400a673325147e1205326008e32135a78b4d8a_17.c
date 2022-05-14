QTN2QT(QTNode *in)
{
	TSQuery		out;
	int			len;
	int			sumlen = 0,
				nnode = 0;
 	QTN2QTState state;
 
 	cntsize(in, &sumlen, &nnode);

	if (TSQUERY_TOO_BIG(nnode, sumlen))
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
				 errmsg("tsquery is too large")));
 	len = COMPUTESIZE(nnode, sumlen);
 
 	out = (TSQuery) palloc0(len);
	SET_VARSIZE(out, len);
	out->size = nnode;

	state.curitem = GETQUERY(out);
	state.operand = state.curoperand = GETOPERAND(out);

	fillQT(&state, in);
	return out;
}
