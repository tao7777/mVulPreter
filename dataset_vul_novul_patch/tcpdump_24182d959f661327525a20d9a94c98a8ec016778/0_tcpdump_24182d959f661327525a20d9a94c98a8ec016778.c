smb_fdata(netdissect_options *ndo,
          const u_char *buf, const char *fmt, const u_char *maxbuf,
          int unicodestr)
{
    static int depth = 0;
    char s[128];
    char *p;

    while (*fmt) {
	switch (*fmt) {
	case '*':
	    fmt++;
 	    while (buf < maxbuf) {
 		const u_char *buf2;
 		depth++;
		/* Not sure how this relates with the protocol specification,
		 * but in order to avoid stack exhaustion recurse at most that
		 * many levels.
		 */
		if (depth == 10)
			ND_PRINT((ndo, "(too many nested levels, not recursing)"));
		else
			buf2 = smb_fdata(ndo, buf, fmt, maxbuf, unicodestr);
 		depth--;
 		if (buf2 == NULL)
 		    return(NULL);
		if (buf2 == buf)
		    return(buf);
		buf = buf2;
	    }
	    return(buf);

	case '|':
	    fmt++;
	    if (buf >= maxbuf)
		return(buf);
	    break;

	case '%':
	    fmt++;
	    buf = maxbuf;
	    break;

	case '#':
	    fmt++;
	    return(buf);
	    break;

	case '[':
	    fmt++;
	    if (buf >= maxbuf)
		return(buf);
	    memset(s, 0, sizeof(s));
	    p = strchr(fmt, ']');
	    if ((size_t)(p - fmt + 1) > sizeof(s)) {
		/* overrun */
		return(buf);
	    }
	    strncpy(s, fmt, p - fmt);
	    s[p - fmt] = '\0';
	    fmt = p + 1;
	    buf = smb_fdata1(ndo, buf, s, maxbuf, unicodestr);
	    if (buf == NULL)
		return(NULL);
	    break;

	default:
	    ND_PRINT((ndo, "%c", *fmt));
	    fmt++;
	    break;
	}
    }
    if (!depth && buf < maxbuf) {
	size_t len = PTR_DIFF(maxbuf, buf);
	ND_PRINT((ndo, "Data: (%lu bytes)\n", (unsigned long)len));
	smb_print_data(ndo, buf, len);
	return(buf + len);
    }
    return(buf);
}
