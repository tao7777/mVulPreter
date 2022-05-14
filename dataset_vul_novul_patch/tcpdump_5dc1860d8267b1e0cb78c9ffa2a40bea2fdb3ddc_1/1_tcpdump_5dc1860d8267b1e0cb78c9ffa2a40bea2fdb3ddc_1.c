pimv1_join_prune_print(netdissect_options *ndo,
                       register const u_char *bp, register u_int len)
{
	int ngroups, njoin, nprune;
	int njp;

	/* If it's a single group and a single source, use 1-line output. */
	if (ND_TTEST2(bp[0], 30) && bp[11] == 1 &&
	    ((njoin = EXTRACT_16BITS(&bp[20])) + EXTRACT_16BITS(&bp[22])) == 1) {
		int hold;

		ND_PRINT((ndo, " RPF %s ", ipaddr_string(ndo, bp)));
		hold = EXTRACT_16BITS(&bp[6]);
		if (hold != 180) {
			ND_PRINT((ndo, "Hold "));
			unsigned_relts_print(ndo, hold);
		}
		ND_PRINT((ndo, "%s (%s/%d, %s", njoin ? "Join" : "Prune",
		ipaddr_string(ndo, &bp[26]), bp[25] & 0x3f,
		ipaddr_string(ndo, &bp[12])));
		if (EXTRACT_32BITS(&bp[16]) != 0xffffffff)
			ND_PRINT((ndo, "/%s", ipaddr_string(ndo, &bp[16])));
		ND_PRINT((ndo, ") %s%s %s",
		    (bp[24] & 0x01) ? "Sparse" : "Dense",
		    (bp[25] & 0x80) ? " WC" : "",
		    (bp[25] & 0x40) ? "RP" : "SPT"));
 		return;
 	}
 
 	ND_TCHECK2(bp[0], sizeof(struct in_addr));
 	if (ndo->ndo_vflag > 1)
 		ND_PRINT((ndo, "\n"));
 	ND_PRINT((ndo, " Upstream Nbr: %s", ipaddr_string(ndo, bp)));
	ND_TCHECK2(bp[6], 2);
 	if (ndo->ndo_vflag > 1)
 		ND_PRINT((ndo, "\n"));
 	ND_PRINT((ndo, " Hold time: "));
	unsigned_relts_print(ndo, EXTRACT_16BITS(&bp[6]));
 	if (ndo->ndo_vflag < 2)
 		return;
	bp += 8;
	len -= 8;
 
 	ND_TCHECK2(bp[0], 4);
 	ngroups = bp[3];
 	bp += 4;
	len -= 4;
	while (ngroups--) {
		/*
 		 * XXX - does the address have length "addrlen" and the
 		 * mask length "maddrlen"?
 		 */
 		ND_TCHECK2(bp[0], sizeof(struct in_addr));
 		ND_PRINT((ndo, "\n\tGroup: %s", ipaddr_string(ndo, bp)));
		ND_TCHECK2(bp[4], sizeof(struct in_addr));
		if (EXTRACT_32BITS(&bp[4]) != 0xffffffff)
			ND_PRINT((ndo, "/%s", ipaddr_string(ndo, &bp[4])));
		ND_TCHECK2(bp[8], 4);
		njoin = EXTRACT_16BITS(&bp[8]);
		nprune = EXTRACT_16BITS(&bp[10]);
 		ND_PRINT((ndo, " joined: %d pruned: %d", njoin, nprune));
		bp += 12;
		len -= 12;
 		for (njp = 0; njp < (njoin + nprune); njp++) {
 			const char *type;
 
 			if (njp < njoin)
 				type = "Join ";
 			else
 				type = "Prune";
 			ND_TCHECK2(bp[0], 6);
 			ND_PRINT((ndo, "\n\t%s %s%s%s%s/%d", type,
 			    (bp[0] & 0x01) ? "Sparse " : "Dense ",
 			    (bp[1] & 0x80) ? "WC " : "",
 			    (bp[1] & 0x40) ? "RP " : "SPT ",
			ipaddr_string(ndo, &bp[2]), bp[1] & 0x3f));
 			bp += 6;
 			len -= 6;
 		}
	}
	return;
trunc:
	ND_PRINT((ndo, "[|pim]"));
	return;
}
