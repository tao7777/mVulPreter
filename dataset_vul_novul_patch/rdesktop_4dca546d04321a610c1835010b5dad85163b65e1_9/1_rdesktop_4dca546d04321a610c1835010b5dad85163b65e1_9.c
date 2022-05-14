rdp_in_unistr(STREAM s, int in_len, char **string, uint32 * str_size)
{
	static iconv_t icv_utf16_to_local;
 	size_t ibl, obl;
 	char *pin, *pout;
 
 	if (!icv_utf16_to_local)
 	{
		icv_utf16_to_local = iconv_open(g_codepage, WINDOWS_CODEPAGE);
		if (icv_utf16_to_local == (iconv_t) - 1)
		{
			logger(Protocol, Error, "rdp_in_unistr(), iconv_open[%s -> %s] fail %p",
			       WINDOWS_CODEPAGE, g_codepage, icv_utf16_to_local);
			abort();
		}
	}

	/* Dynamic allocate of destination string if not provided */
	if (*string == NULL)
	{

		*string = xmalloc(in_len * 2);
		*str_size = in_len * 2;
	}

	ibl = in_len;
	obl = *str_size - 1;
	pin = (char *) s->p;
	pout = *string;

	if (iconv(icv_utf16_to_local, (char **) &pin, &ibl, &pout, &obl) == (size_t) - 1)
	{
		if (errno == E2BIG)
		{
			logger(Protocol, Warning,
			       "rdp_in_unistr(), server sent an unexpectedly long string, truncating");
		}
		else
		{
			logger(Protocol, Warning, "rdp_in_unistr(), iconv fail, errno %d", errno);

			free(*string);
			*string = NULL;
			*str_size = 0;
		}
		abort();
	}

	/* we must update the location of the current STREAM for future reads of s->p */
	s->p += in_len;

	*pout = 0;

	if (*string)
		*str_size = pout - *string;
}
