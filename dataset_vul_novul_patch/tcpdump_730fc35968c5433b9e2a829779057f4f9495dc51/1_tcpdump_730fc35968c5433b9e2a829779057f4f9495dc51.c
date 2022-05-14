le64addr_string(netdissect_options *ndo, const u_char *ep)
{
 	const unsigned int len = 8;
 	register u_int i;
 	register char *cp;
	register struct enamemem *tp;
 	char buf[BUFSIZE];
 
 	tp = lookup_bytestring(ndo, ep, len);
	if (tp->e_name)
		return (tp->e_name);
 
 	cp = buf;
 	for (i = len; i > 0 ; --i) {
		*cp++ = hex[*(ep + i - 1) >> 4];
		*cp++ = hex[*(ep + i - 1) & 0xf];
		*cp++ = ':';
	}
	cp --;
 
 	*cp = '\0';
 
	tp->e_name = strdup(buf);
	if (tp->e_name == NULL)
 		(*ndo->ndo_error)(ndo, "le64addr_string: strdup(buf)");
 
	return (tp->e_name);
 }
