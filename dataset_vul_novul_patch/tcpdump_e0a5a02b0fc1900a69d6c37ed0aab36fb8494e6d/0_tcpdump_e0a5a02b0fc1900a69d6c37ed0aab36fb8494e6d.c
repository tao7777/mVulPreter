ikev1_attr_print(netdissect_options *ndo, const u_char *p, const u_char *ep)
ikev1_attr_print(netdissect_options *ndo, const u_char *p, const u_char *ep2)
 {
 	int totlen;
 	uint32_t t;
 
	ND_TCHECK(p[0]);
 	if (p[0] & 0x80)
 		totlen = 4;
	else {
		ND_TCHECK_16BITS(&p[2]);
 		totlen = 4 + EXTRACT_16BITS(&p[2]);
	}
	if (ep2 < p + totlen) {
 		ND_PRINT((ndo,"[|attr]"));
		return ep2 + 1;
 	}
 
	ND_TCHECK_16BITS(&p[0]);
 	ND_PRINT((ndo,"("));
 	t = EXTRACT_16BITS(&p[0]) & 0x7fff;
 	ND_PRINT((ndo,"type=#%d ", t));
 	if (p[0] & 0x80) {
 		ND_PRINT((ndo,"value="));
 		t = p[2];
		if (!rawprint(ndo, (const uint8_t *)&p[2], 2)) {
			ND_PRINT((ndo,")"));
			goto trunc;
		}
 	} else {
		ND_PRINT((ndo,"len=%d value=", totlen - 4));
		if (!rawprint(ndo, (const uint8_t *)&p[4], totlen - 4)) {
			ND_PRINT((ndo,")"));
			goto trunc;
		}
 	}
 	ND_PRINT((ndo,")"));
 	return p + totlen;

trunc:
	return NULL;
 }
