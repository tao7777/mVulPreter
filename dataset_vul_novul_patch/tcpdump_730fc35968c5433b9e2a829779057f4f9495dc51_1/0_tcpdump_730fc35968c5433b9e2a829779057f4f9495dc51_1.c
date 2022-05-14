 lookup_bytestring(netdissect_options *ndo, register const u_char *bs,
 		  const unsigned int nlen)
 {
	struct bsnamemem *tp;
 	register u_int i, j, k;
 
 	if (nlen >= 6) {
		k = (bs[0] << 8) | bs[1];
		j = (bs[2] << 8) | bs[3];
		i = (bs[4] << 8) | bs[5];
	} else if (nlen >= 4) {
		k = (bs[0] << 8) | bs[1];
		j = (bs[2] << 8) | bs[3];
		i = 0;
	} else
 		i = j = k = 0;
 
 	tp = &bytestringtable[(i ^ j) & (HASHNAMESIZE-1)];
	while (tp->bs_nxt)
		if (nlen == tp->bs_nbytes &&
		    tp->bs_addr0 == i &&
		    tp->bs_addr1 == j &&
		    tp->bs_addr2 == k &&
		    memcmp((const char *)bs, (const char *)(tp->bs_bytes), nlen) == 0)
 			return tp;
 		else
			tp = tp->bs_nxt;
 
	tp->bs_addr0 = i;
	tp->bs_addr1 = j;
	tp->bs_addr2 = k;
 
	tp->bs_bytes = (u_char *) calloc(1, nlen + 1);
	if (tp->bs_bytes == NULL)
 		(*ndo->ndo_error)(ndo, "lookup_bytestring: calloc");
 
	memcpy(tp->bs_bytes, bs, nlen);
	tp->bs_nbytes = nlen;
	tp->bs_nxt = (struct bsnamemem *)calloc(1, sizeof(*tp));
	if (tp->bs_nxt == NULL)
 		(*ndo->ndo_error)(ndo, "lookup_bytestring: calloc");
 
 	return tp;
}
