mobility_opt_print(netdissect_options *ndo,
                   const u_char *bp, const unsigned len)
{
	unsigned i, optlen;

	for (i = 0; i < len; i += optlen) {
		ND_TCHECK(bp[i]);
		if (bp[i] == IP6MOPT_PAD1)
			optlen = 1;
		else {
			if (i + 1 < len) {
				ND_TCHECK(bp[i + 1]);
				optlen = bp[i + 1] + 2;
			}
			else
				goto trunc;
		}
		if (i + optlen > len)
			goto trunc;
		ND_TCHECK(bp[i + optlen]);

		switch (bp[i]) {
		case IP6MOPT_PAD1:
			ND_PRINT((ndo, "(pad1)"));
			break;
		case IP6MOPT_PADN:
			if (len - i < IP6MOPT_MINLEN) {
				ND_PRINT((ndo, "(padn: trunc)"));
				goto trunc;
			}
			ND_PRINT((ndo, "(padn)"));
			break;
		case IP6MOPT_REFRESH:
			if (len - i < IP6MOPT_REFRESH_MINLEN) {
				ND_PRINT((ndo, "(refresh: trunc)"));
				goto trunc;
			}
			/* units of 4 secs */
			ND_TCHECK_16BITS(&bp[i+2]);
			ND_PRINT((ndo, "(refresh: %u)",
				EXTRACT_16BITS(&bp[i+2]) << 2));
			break;
		case IP6MOPT_ALTCOA:
			if (len - i < IP6MOPT_ALTCOA_MINLEN) {
 				ND_PRINT((ndo, "(altcoa: trunc)"));
 				goto trunc;
 			}
			ND_TCHECK_128BITS(&bp[i+2]);
 			ND_PRINT((ndo, "(alt-CoA: %s)", ip6addr_string(ndo, &bp[i+2])));
 			break;
 		case IP6MOPT_NONCEID:
			if (len - i < IP6MOPT_NONCEID_MINLEN) {
				ND_PRINT((ndo, "(ni: trunc)"));
				goto trunc;
			}
			ND_TCHECK_16BITS(&bp[i+2]);
			ND_TCHECK_16BITS(&bp[i+4]);
			ND_PRINT((ndo, "(ni: ho=0x%04x co=0x%04x)",
				EXTRACT_16BITS(&bp[i+2]),
				EXTRACT_16BITS(&bp[i+4])));
			break;
		case IP6MOPT_AUTH:
			if (len - i < IP6MOPT_AUTH_MINLEN) {
				ND_PRINT((ndo, "(auth: trunc)"));
				goto trunc;
			}
			ND_PRINT((ndo, "(auth)"));
			break;
		default:
			if (len - i < IP6MOPT_MINLEN) {
				ND_PRINT((ndo, "(sopt_type %u: trunc)", bp[i]));
				goto trunc;
			}
			ND_PRINT((ndo, "(type-0x%02x: len=%u)", bp[i], bp[i + 1]));
			break;
		}
	}
	return 0;

trunc:
	return 1;
}
