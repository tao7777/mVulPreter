vrrp_print(netdissect_options *ndo,
           register const u_char *bp, register u_int len,
           register const u_char *bp2, int ttl)
{
	int version, type, auth_type = VRRP_AUTH_NONE; /* keep compiler happy */
	const char *type_s;

	ND_TCHECK(bp[0]);
	version = (bp[0] & 0xf0) >> 4;
	type = bp[0] & 0x0f;
	type_s = tok2str(type2str, "unknown type (%u)", type);
	ND_PRINT((ndo, "VRRPv%u, %s", version, type_s));
	if (ttl != 255)
		ND_PRINT((ndo, ", (ttl %u)", ttl));
	if (version < 2 || version > 3 || type != VRRP_TYPE_ADVERTISEMENT)
		return;
	ND_TCHECK(bp[2]);
	ND_PRINT((ndo, ", vrid %u, prio %u", bp[1], bp[2]));
	ND_TCHECK(bp[5]);

	if (version == 2) {
		auth_type = bp[4];
		ND_PRINT((ndo, ", authtype %s", tok2str(auth2str, NULL, auth_type)));
		ND_PRINT((ndo, ", intvl %us, length %u", bp[5], len));
	} else { /* version == 3 */
		uint16_t intvl = (bp[4] & 0x0f) << 8 | bp[5];
		ND_PRINT((ndo, ", intvl %ucs, length %u", intvl, len));
	}

	if (ndo->ndo_vflag) {
		int naddrs = bp[3];
		int i;
		char c;

		if (version == 2 && ND_TTEST2(bp[0], len)) {
			struct cksum_vec vec[1];
 
 			vec[0].ptr = bp;
 			vec[0].len = len;
			if (in_cksum(vec, 1)) {
				ND_TCHECK_16BITS(&bp[6]);
 				ND_PRINT((ndo, ", (bad vrrp cksum %x)",
 					EXTRACT_16BITS(&bp[6])));
			}
 		}
 
 		if (version == 3 && ND_TTEST2(bp[0], len)) {
			uint16_t cksum = nextproto4_cksum(ndo, (const struct ip *)bp2, bp,
				len, len, IPPROTO_VRRP);
			if (cksum)
				ND_PRINT((ndo, ", (bad vrrp cksum %x)",
					EXTRACT_16BITS(&bp[6])));
		}

		ND_PRINT((ndo, ", addrs"));
		if (naddrs > 1)
			ND_PRINT((ndo, "(%d)", naddrs));
		ND_PRINT((ndo, ":"));
		c = ' ';
		bp += 8;
		for (i = 0; i < naddrs; i++) {
			ND_TCHECK(bp[3]);
			ND_PRINT((ndo, "%c%s", c, ipaddr_string(ndo, bp)));
			c = ',';
			bp += 4;
		}
		if (version == 2 && auth_type == VRRP_AUTH_SIMPLE) { /* simple text password */
			ND_TCHECK(bp[7]);
			ND_PRINT((ndo, " auth \""));
			if (fn_printn(ndo, bp, 8, ndo->ndo_snapend)) {
				ND_PRINT((ndo, "\""));
				goto trunc;
			}
			ND_PRINT((ndo, "\""));
		}
	}
	return;
trunc:
	ND_PRINT((ndo, "[|vrrp]"));
}
