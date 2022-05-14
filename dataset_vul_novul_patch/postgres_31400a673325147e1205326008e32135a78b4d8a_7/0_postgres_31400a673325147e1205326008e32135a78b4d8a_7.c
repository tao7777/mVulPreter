lquery_in(PG_FUNCTION_ARGS)
{
	char	   *buf = (char *) PG_GETARG_POINTER(0);
	char	   *ptr;
	int			num = 0,
				totallen = 0,
				numOR = 0;
	int			state = LQPRS_WAITLEVEL;
	lquery	   *result;
	nodeitem   *lptr = NULL;
	lquery_level *cur,
			   *curqlevel,
			   *tmpql;
	lquery_variant *lrptr = NULL;
	bool		hasnot = false;
	bool		wasbad = false;
	int			charlen;
	int			pos = 0;

	ptr = buf;
	while (*ptr)
	{
		charlen = pg_mblen(ptr);

		if (charlen == 1)
		{
			if (t_iseq(ptr, '.'))
				num++;
			else if (t_iseq(ptr, '|'))
				numOR++;
		}

		ptr += charlen;
 	}
 
 	num++;
	if (num > MaxAllocSize / ITEMSIZE)
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
			 errmsg("number of levels (%d) exceeds the maximum allowed (%d)",
					num, (int) (MaxAllocSize / ITEMSIZE))));
 	curqlevel = tmpql = (lquery_level *) palloc0(ITEMSIZE * num);
 	ptr = buf;
 	while (*ptr)
	{
		charlen = pg_mblen(ptr);

		if (state == LQPRS_WAITLEVEL)
		{
			if (ISALNUM(ptr))
			{
				GETVAR(curqlevel) = lptr = (nodeitem *) palloc0(sizeof(nodeitem) * (numOR + 1));
				lptr->start = ptr;
				state = LQPRS_WAITDELIM;
				curqlevel->numvar = 1;
			}
			else if (charlen == 1 && t_iseq(ptr, '!'))
			{
				GETVAR(curqlevel) = lptr = (nodeitem *) palloc0(sizeof(nodeitem) * (numOR + 1));
				lptr->start = ptr + 1;
				state = LQPRS_WAITDELIM;
				curqlevel->numvar = 1;
				curqlevel->flag |= LQL_NOT;
				hasnot = true;
			}
			else if (charlen == 1 && t_iseq(ptr, '*'))
				state = LQPRS_WAITOPEN;
			else
				UNCHAR;
		}
		else if (state == LQPRS_WAITVAR)
		{
			if (ISALNUM(ptr))
			{
				lptr++;
				lptr->start = ptr;
				state = LQPRS_WAITDELIM;
				curqlevel->numvar++;
			}
			else
				UNCHAR;
		}
		else if (state == LQPRS_WAITDELIM)
		{
			if (charlen == 1 && t_iseq(ptr, '@'))
			{
				if (lptr->start == ptr)
					UNCHAR;
				lptr->flag |= LVAR_INCASE;
				curqlevel->flag |= LVAR_INCASE;
			}
			else if (charlen == 1 && t_iseq(ptr, '*'))
			{
				if (lptr->start == ptr)
					UNCHAR;
				lptr->flag |= LVAR_ANYEND;
				curqlevel->flag |= LVAR_ANYEND;
			}
			else if (charlen == 1 && t_iseq(ptr, '%'))
			{
				if (lptr->start == ptr)
					UNCHAR;
				lptr->flag |= LVAR_SUBLEXEME;
				curqlevel->flag |= LVAR_SUBLEXEME;
			}
			else if (charlen == 1 && t_iseq(ptr, '|'))
			{
				lptr->len = ptr - lptr->start -
					((lptr->flag & LVAR_SUBLEXEME) ? 1 : 0) -
					((lptr->flag & LVAR_INCASE) ? 1 : 0) -
					((lptr->flag & LVAR_ANYEND) ? 1 : 0);
				if (lptr->wlen > 255)
					ereport(ERROR,
							(errcode(ERRCODE_NAME_TOO_LONG),
							 errmsg("name of level is too long"),
							 errdetail("Name length is %d, must "
									   "be < 256, in position %d.",
									   lptr->wlen, pos)));

				state = LQPRS_WAITVAR;
			}
			else if (charlen == 1 && t_iseq(ptr, '.'))
			{
				lptr->len = ptr - lptr->start -
					((lptr->flag & LVAR_SUBLEXEME) ? 1 : 0) -
					((lptr->flag & LVAR_INCASE) ? 1 : 0) -
					((lptr->flag & LVAR_ANYEND) ? 1 : 0);
				if (lptr->wlen > 255)
					ereport(ERROR,
							(errcode(ERRCODE_NAME_TOO_LONG),
							 errmsg("name of level is too long"),
							 errdetail("Name length is %d, must "
									   "be < 256, in position %d.",
									   lptr->wlen, pos)));

				state = LQPRS_WAITLEVEL;
				curqlevel = NEXTLEV(curqlevel);
			}
			else if (ISALNUM(ptr))
			{
				if (lptr->flag)
					UNCHAR;
			}
			else
				UNCHAR;
		}
		else if (state == LQPRS_WAITOPEN)
		{
			if (charlen == 1 && t_iseq(ptr, '{'))
				state = LQPRS_WAITFNUM;
			else if (charlen == 1 && t_iseq(ptr, '.'))
			{
				curqlevel->low = 0;
				curqlevel->high = 0xffff;
				curqlevel = NEXTLEV(curqlevel);
				state = LQPRS_WAITLEVEL;
			}
			else
				UNCHAR;
		}
		else if (state == LQPRS_WAITFNUM)
		{
			if (charlen == 1 && t_iseq(ptr, ','))
				state = LQPRS_WAITSNUM;
			else if (t_isdigit(ptr))
			{
				curqlevel->low = atoi(ptr);
				state = LQPRS_WAITND;
			}
			else
				UNCHAR;
		}
		else if (state == LQPRS_WAITSNUM)
		{
			if (t_isdigit(ptr))
			{
				curqlevel->high = atoi(ptr);
				state = LQPRS_WAITCLOSE;
			}
			else if (charlen == 1 && t_iseq(ptr, '}'))
			{
				curqlevel->high = 0xffff;
				state = LQPRS_WAITEND;
			}
			else
				UNCHAR;
		}
		else if (state == LQPRS_WAITCLOSE)
		{
			if (charlen == 1 && t_iseq(ptr, '}'))
				state = LQPRS_WAITEND;
			else if (!t_isdigit(ptr))
				UNCHAR;
		}
		else if (state == LQPRS_WAITND)
		{
			if (charlen == 1 && t_iseq(ptr, '}'))
			{
				curqlevel->high = curqlevel->low;
				state = LQPRS_WAITEND;
			}
			else if (charlen == 1 && t_iseq(ptr, ','))
				state = LQPRS_WAITSNUM;
			else if (!t_isdigit(ptr))
				UNCHAR;
		}
