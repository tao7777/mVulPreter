l_strnstart(const char *tstr1, u_int tl1, const char *str2, u_int l2)
l_strnstart(netdissect_options *ndo, const char *tstr1, u_int tl1,
    const char *str2, u_int l2)
 {
	if (!ND_TTEST2(*str2, tl1)) {
		/*
		 * We don't have tl1 bytes worth of captured data
		 * for the string, so we can't check for this
		 * string.
		 */
		return 0;
	}
 	if (tl1 > l2)
 		return 0;
 
	return (strncmp(tstr1, str2, tl1) == 0 ? 1 : 0);
}
