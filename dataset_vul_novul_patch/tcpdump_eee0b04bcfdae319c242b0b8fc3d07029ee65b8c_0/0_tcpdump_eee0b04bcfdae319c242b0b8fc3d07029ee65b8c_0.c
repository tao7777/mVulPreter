 ip_printroute(netdissect_options *ndo,
               register const u_char *cp, u_int length)
 {
	register u_int ptr;
	register u_int len;
 
 	if (length < 3) {
 		ND_PRINT((ndo, " [bad length %u]", length));
		return (0);
 	}
 	if ((length + 1) & 3)
 		ND_PRINT((ndo, " [bad length %u]", length));
	ND_TCHECK(cp[2]);
 	ptr = cp[2] - 1;
 	if (ptr < 3 || ((ptr + 1) & 3) || ptr > length + 1)
 		ND_PRINT((ndo, " [bad ptr %u]", cp[2]));
 
 	for (len = 3; len < length; len += 4) {
		ND_TCHECK2(cp[len], 4);
 		ND_PRINT((ndo, " %s", ipaddr_string(ndo, &cp[len])));
 		if (ptr > len)
 			ND_PRINT((ndo, ","));
 	}
	return (0);

trunc:
	return (-1);
 }
