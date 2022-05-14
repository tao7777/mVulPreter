 ikev1_attrmap_print(netdissect_options *ndo,
		    const u_char *p, const u_char *ep2,
 		    const struct attrmap *map, size_t nmap)
 {
 	int totlen;
 	uint32_t t, v;
 
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
 	if (map && t < nmap && map[t].type)
		ND_PRINT((ndo,"type=%s ", map[t].type));
	else
 		ND_PRINT((ndo,"type=#%d ", t));
 	if (p[0] & 0x80) {
 		ND_PRINT((ndo,"value="));
		ND_TCHECK_16BITS(&p[2]);
 		v = EXTRACT_16BITS(&p[2]);
 		if (map && t < nmap && v < map[t].nvalue && map[t].value[v])
 			ND_PRINT((ndo,"%s", map[t].value[v]));
		else {
			if (!rawprint(ndo, (const uint8_t *)&p[2], 2)) {
				ND_PRINT((ndo,")"));
				goto trunc;
			}
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
