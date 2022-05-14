ikev1_attr_print(netdissect_options *ndo, const u_char *p, const u_char *ep)
 {
 	int totlen;
 	uint32_t t;
 
 	if (p[0] & 0x80)
 		totlen = 4;
	else
 		totlen = 4 + EXTRACT_16BITS(&p[2]);
	if (ep < p + totlen) {
 		ND_PRINT((ndo,"[|attr]"));
		return ep + 1;
 	}
 
 	ND_PRINT((ndo,"("));
 	t = EXTRACT_16BITS(&p[0]) & 0x7fff;
 	ND_PRINT((ndo,"type=#%d ", t));
 	if (p[0] & 0x80) {
 		ND_PRINT((ndo,"value="));
 		t = p[2];
		rawprint(ndo, (const uint8_t *)&p[2], 2);
 	} else {
		ND_PRINT((ndo,"len=%d value=", EXTRACT_16BITS(&p[2])));
		rawprint(ndo, (const uint8_t *)&p[4], EXTRACT_16BITS(&p[2]));
 	}
 	ND_PRINT((ndo,")"));
 	return p + totlen;
 }
