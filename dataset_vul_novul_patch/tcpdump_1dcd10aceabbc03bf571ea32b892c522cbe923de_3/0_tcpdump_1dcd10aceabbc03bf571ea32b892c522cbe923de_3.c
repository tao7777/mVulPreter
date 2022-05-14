gre_print_0(netdissect_options *ndo, const u_char *bp, u_int length)
{
	u_int len = length;
	uint16_t flags, prot;

	/* 16 bits ND_TCHECKed in gre_print() */
	flags = EXTRACT_16BITS(bp);
        if (ndo->ndo_vflag)
            ND_PRINT((ndo, ", Flags [%s]",
                   bittok2str(gre_flag_values,"none",flags)));

	len -= 2;
	bp += 2;

	ND_TCHECK2(*bp, 2);
	if (len < 2)
		goto trunc;
	prot = EXTRACT_16BITS(bp);
	len -= 2;
	bp += 2;

	if ((flags & GRE_CP) | (flags & GRE_RP)) {
		ND_TCHECK2(*bp, 2);
		if (len < 2)
			goto trunc;
		if (ndo->ndo_vflag)
			ND_PRINT((ndo, ", sum 0x%x", EXTRACT_16BITS(bp)));
		bp += 2;
		len -= 2;

		ND_TCHECK2(*bp, 2);
		if (len < 2)
			goto trunc;
		ND_PRINT((ndo, ", off 0x%x", EXTRACT_16BITS(bp)));
		bp += 2;
		len -= 2;
	}

	if (flags & GRE_KP) {
		ND_TCHECK2(*bp, 4);
		if (len < 4)
			goto trunc;
		ND_PRINT((ndo, ", key=0x%x", EXTRACT_32BITS(bp)));
		bp += 4;
		len -= 4;
	}

	if (flags & GRE_SP) {
		ND_TCHECK2(*bp, 4);
		if (len < 4)
			goto trunc;
		ND_PRINT((ndo, ", seq %u", EXTRACT_32BITS(bp)));
		bp += 4;
		len -= 4;
	}

	if (flags & GRE_RP) {
		for (;;) {
			uint16_t af;
			uint8_t sreoff;
			uint8_t srelen;

			ND_TCHECK2(*bp, 4);
			if (len < 4)
				goto trunc;
			af = EXTRACT_16BITS(bp);
			sreoff = *(bp + 2);
			srelen = *(bp + 3);
			bp += 4;
			len -= 4;

			if (af == 0 && srelen == 0)
				break;

			if (!gre_sre_print(ndo, af, sreoff, srelen, bp, len))
				goto trunc;

			if (len < srelen)
				goto trunc;
			bp += srelen;
			len -= srelen;
		}
	}

        if (ndo->ndo_eflag)
            ND_PRINT((ndo, ", proto %s (0x%04x)",
                   tok2str(ethertype_values,"unknown",prot),
                   prot));

        ND_PRINT((ndo, ", length %u",length));

        if (ndo->ndo_vflag < 1)
            ND_PRINT((ndo, ": ")); /* put in a colon as protocol demarc */
        else
            ND_PRINT((ndo, "\n\t")); /* if verbose go multiline */

	switch (prot) {
	case ETHERTYPE_IP:
	        ip_print(ndo, bp, len);
		break;
	case ETHERTYPE_IPV6:
		ip6_print(ndo, bp, len);
		break;
	case ETHERTYPE_MPLS:
		mpls_print(ndo, bp, len);
		break;
	case ETHERTYPE_IPX:
		ipx_print(ndo, bp, len);
		break;
	case ETHERTYPE_ATALK:
 		atalk_print(ndo, bp, len);
 		break;
 	case ETHERTYPE_GRE_ISO:
		isoclns_print(ndo, bp, len);
 		break;
 	case ETHERTYPE_TEB:
 		ether_print(ndo, bp, len, ndo->ndo_snapend - bp, NULL, NULL);
		break;
	default:
		ND_PRINT((ndo, "gre-proto-0x%x", prot));
	}
	return;

trunc:
	ND_PRINT((ndo, "%s", tstr));
}
