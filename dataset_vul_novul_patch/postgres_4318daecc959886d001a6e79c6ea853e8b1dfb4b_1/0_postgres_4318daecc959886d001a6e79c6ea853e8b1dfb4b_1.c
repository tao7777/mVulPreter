DecodeTime(char *str, int *tmask, struct tm * tm, fsec_t *fsec)
{
	char	   *cp;

	*tmask = DTK_TIME_M;

	tm->tm_hour = strtol(str, &cp, 10);
	if (*cp != ':')
		return -1;
	str = cp + 1;
	tm->tm_min = strtol(str, &cp, 10);
	if (*cp == '\0')
	{
		tm->tm_sec = 0;
		*fsec = 0;
	}
	else if (*cp != ':')
		return -1;
	else
	{
		str = cp + 1;
		tm->tm_sec = strtol(str, &cp, 10);
		if (*cp == '\0')
			*fsec = 0;
 		else if (*cp == '.')
 		{
 #ifdef HAVE_INT64_TIMESTAMP
			char		fstr[7];
			int			i;

			cp++;
 
 			/*
			 * OK, we have at most six digits to care about. Let's construct a
			 * string with those digits, zero-padded on the right, and then do
			 * the conversion to an integer.
			 *
			 * XXX This truncates the seventh digit, unlike rounding it as do
			 * the backend and the !HAVE_INT64_TIMESTAMP case.
 			 */
			for (i = 0; i < 6; i++)
				fstr[i] = *cp != '\0' ? *cp++ : '0';
			fstr[i] = '\0';
 			*fsec = strtol(fstr, &cp, 10);
 #else
 			str = cp;
			*fsec = strtod(str, &cp);
#endif
			if (*cp != '\0')
				return -1;
		}
		else
			return -1;
	}

	/* do a sanity check */
#ifdef HAVE_INT64_TIMESTAMP
	if (tm->tm_hour < 0 || tm->tm_min < 0 || tm->tm_min > 59 ||
		tm->tm_sec < 0 || tm->tm_sec > 59 || *fsec >= USECS_PER_SEC)
		return -1;
#else
	if (tm->tm_hour < 0 || tm->tm_min < 0 || tm->tm_min > 59 ||
		tm->tm_sec < 0 || tm->tm_sec > 59 || *fsec >= 1)
		return -1;
#endif

	return 0;
}	/* DecodeTime() */
