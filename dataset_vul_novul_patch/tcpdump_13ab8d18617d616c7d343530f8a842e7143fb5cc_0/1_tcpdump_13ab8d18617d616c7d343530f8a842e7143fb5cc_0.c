 atmarp_print(netdissect_options *ndo,
 	     const u_char *bp, u_int length, u_int caplen)
{
	const struct atmarp_pkthdr *ap;
	u_short pro, hrd, op;

	ap = (const struct atmarp_pkthdr *)bp;
	ND_TCHECK(*ap);

	hrd = ATMHRD(ap);
	pro = ATMPRO(ap);
	op = ATMOP(ap);

	if (!ND_TTEST2(*aar_tpa(ap), ATMTPROTO_LEN(ap))) {
		ND_PRINT((ndo, "%s", tstr));
		ND_DEFAULTPRINT((const u_char *)ap, length);
		return;
	}

        if (!ndo->ndo_eflag) {
            ND_PRINT((ndo, "ARP, "));
        }

	if ((pro != ETHERTYPE_IP && pro != ETHERTYPE_TRAIL) ||
	    ATMSPROTO_LEN(ap) != 4 ||
            ATMTPROTO_LEN(ap) != 4 ||
            ndo->ndo_vflag) {
                ND_PRINT((ndo, "%s, %s (len %u/%u)",
                          tok2str(arphrd_values, "Unknown Hardware (%u)", hrd),
                          tok2str(ethertype_values, "Unknown Protocol (0x%04x)", pro),
                          ATMSPROTO_LEN(ap),
                          ATMTPROTO_LEN(ap)));

                /* don't know know about the address formats */
                if (!ndo->ndo_vflag) {
                    goto out;
                }
	}

        /* print operation */
        ND_PRINT((ndo, "%s%s ",
               ndo->ndo_vflag ? ", " : "",
               tok2str(arpop_values, "Unknown (%u)", op)));

 	switch (op) {
 
 	case ARPOP_REQUEST:
		ND_PRINT((ndo, "who-has %s", ipaddr_string(ndo, ATMTPA(ap))));
 		if (ATMTHRD_LEN(ap) != 0) {
 			ND_PRINT((ndo, " ("));
 			atmarp_addr_print(ndo, ATMTHA(ap), ATMTHRD_LEN(ap),
 			    ATMTSA(ap), ATMTSLN(ap));
 			ND_PRINT((ndo, ")"));
 		}
		ND_PRINT((ndo, "tell %s", ipaddr_string(ndo, ATMSPA(ap))));
 		break;
 
 	case ARPOP_REPLY:
		ND_PRINT((ndo, "%s is-at ", ipaddr_string(ndo, ATMSPA(ap))));
 		atmarp_addr_print(ndo, ATMSHA(ap), ATMSHRD_LEN(ap), ATMSSA(ap),
                                   ATMSSLN(ap));
 		break;

	case ARPOP_INVREQUEST:
		ND_PRINT((ndo, "who-is "));
		atmarp_addr_print(ndo, ATMTHA(ap), ATMTHRD_LEN(ap), ATMTSA(ap),
		    ATMTSLN(ap));
		ND_PRINT((ndo, " tell "));
		atmarp_addr_print(ndo, ATMSHA(ap), ATMSHRD_LEN(ap), ATMSSA(ap),
		    ATMSSLN(ap));
		break;

 	case ARPOP_INVREPLY:
 		atmarp_addr_print(ndo, ATMSHA(ap), ATMSHRD_LEN(ap), ATMSSA(ap),
 		    ATMSSLN(ap));
		ND_PRINT((ndo, "at %s", ipaddr_string(ndo, ATMSPA(ap))));
 		break;
 
 	case ARPOP_NAK:
		ND_PRINT((ndo, "for %s", ipaddr_string(ndo, ATMSPA(ap))));
 		break;
 
 	default:
		ND_DEFAULTPRINT((const u_char *)ap, caplen);
		return;
	}

 out:
        ND_PRINT((ndo, ", length %u", length));
        return;

trunc:
	ND_PRINT((ndo, "%s", tstr));
}
