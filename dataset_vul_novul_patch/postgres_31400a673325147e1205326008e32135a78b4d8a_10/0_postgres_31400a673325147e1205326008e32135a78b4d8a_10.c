makepol(QPRS_STATE *state)
{
	int32		val = 0,
				type;
	int32		lenval = 0;
	char	   *strval = NULL;
	int32		stack[STACKDEPTH];
 	int32		lenstack = 0;
 	uint16		flag = 0;
 
	/* since this function recurses, it could be driven to stack overflow */
	check_stack_depth();

 	while ((type = gettoken_query(state, &val, &lenval, &strval, &flag)) != END)
 	{
 		switch (type)
		{
			case VAL:
				pushval_asis(state, VAL, strval, lenval, flag);
				while (lenstack && (stack[lenstack - 1] == (int32) '&' ||
									stack[lenstack - 1] == (int32) '!'))
				{
					lenstack--;
					pushquery(state, OPR, stack[lenstack], 0, 0, 0);
				}
				break;
			case OPR:
				if (lenstack && val == (int32) '|')
					pushquery(state, OPR, val, 0, 0, 0);
				else
				{
					if (lenstack == STACKDEPTH)
						/* internal error */
						elog(ERROR, "stack too short");
					stack[lenstack] = val;
					lenstack++;
				}
				break;
			case OPEN:
				if (makepol(state) == ERR)
					return ERR;
				while (lenstack && (stack[lenstack - 1] == (int32) '&' ||
									stack[lenstack - 1] == (int32) '!'))
				{
					lenstack--;
					pushquery(state, OPR, stack[lenstack], 0, 0, 0);
				}
				break;
			case CLOSE:
				while (lenstack)
				{
					lenstack--;
					pushquery(state, OPR, stack[lenstack], 0, 0, 0);
				};
				return END;
				break;
			case ERR:
			default:
				ereport(ERROR,
						(errcode(ERRCODE_SYNTAX_ERROR),
						 errmsg("syntax error")));

				return ERR;

		}
	}
	while (lenstack)
	{
		lenstack--;
		pushquery(state, OPR, stack[lenstack], 0, 0, 0);
	};
	return END;
}
