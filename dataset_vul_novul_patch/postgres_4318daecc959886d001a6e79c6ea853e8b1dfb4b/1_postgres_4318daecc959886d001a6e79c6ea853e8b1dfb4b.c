PGTYPESdate_from_asc(char *str, char **endptr)
{
	date		dDate;
	fsec_t		fsec;
	struct tm	tt,
			   *tm = &tt;
	int			dtype;
 	int			nf;
 	char	   *field[MAXDATEFIELDS];
 	int			ftype[MAXDATEFIELDS];
	char		lowstr[MAXDATELEN + 1];
 	char	   *realptr;
 	char	  **ptr = (endptr != NULL) ? endptr : &realptr;
 
 	bool		EuroDates = FALSE;
 
 	errno = 0;
	if (strlen(str) >= sizeof(lowstr))
 	{
 		errno = PGTYPES_DATE_BAD_DATE;
 		return INT_MIN;
	}

	if (ParseDateTime(str, lowstr, field, ftype, &nf, ptr) != 0 ||
		DecodeDateTime(field, ftype, nf, &dtype, tm, &fsec, EuroDates) != 0)
	{
		errno = PGTYPES_DATE_BAD_DATE;
		return INT_MIN;
	}

	switch (dtype)
	{
		case DTK_DATE:
			break;

		case DTK_EPOCH:
			if (GetEpochTime(tm) < 0)
			{
				errno = PGTYPES_DATE_BAD_DATE;
				return INT_MIN;
			}
			break;

		default:
			errno = PGTYPES_DATE_BAD_DATE;
			return INT_MIN;
	}

	dDate = (date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - date2j(2000, 1, 1));

	return dDate;
}
