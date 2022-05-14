linkaddr_string(netdissect_options *ndo, const u_char *ep,
		const unsigned int type, const unsigned int len)
 {
 	register u_int i;
 	register char *cp;
	register struct bsnamemem *tp;
 
 	if (len == 0)
 		return ("<empty>");

	if (type == LINKADDR_ETHER && len == ETHER_ADDR_LEN)
		return (etheraddr_string(ndo, ep));

	if (type == LINKADDR_FRELAY)
 		return (q922_string(ndo, ep, len));
 
 	tp = lookup_bytestring(ndo, ep, len);
	if (tp->bs_name)
		return (tp->bs_name);
 
	tp->bs_name = cp = (char *)malloc(len*3);
	if (tp->bs_name == NULL)
 		(*ndo->ndo_error)(ndo, "linkaddr_string: malloc");
 	*cp++ = hex[*ep >> 4];
 	*cp++ = hex[*ep++ & 0xf];
	for (i = len-1; i > 0 ; --i) {
		*cp++ = ':';
		*cp++ = hex[*ep >> 4];
 		*cp++ = hex[*ep++ & 0xf];
 	}
 	*cp = '\0';
	return (tp->bs_name);
 }
