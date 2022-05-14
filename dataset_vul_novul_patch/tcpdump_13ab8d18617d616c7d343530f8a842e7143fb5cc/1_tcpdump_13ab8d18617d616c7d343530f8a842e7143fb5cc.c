arp_print(netdissect_options *ndo,
	  const u_char *bp, u_int length, u_int caplen)
{
	const struct arp_pkthdr *ap;
	u_short pro, hrd, op, linkaddr;

	ap = (const struct arp_pkthdr *)bp;
	ND_TCHECK(*ap);

	hrd = HRD(ap);
	pro = PRO(ap);
	op = OP(ap);


        /* if its ATM then call the ATM ARP printer
           for Frame-relay ARP most of the fields
           are similar to Ethernet so overload the Ethernet Printer
           and set the linkaddr type for linkaddr_string(ndo, ) accordingly */

        switch(hrd) {
        case ARPHRD_ATM2225:
            atmarp_print(ndo, bp, length, caplen);
            return;
        case ARPHRD_FRELAY:
            linkaddr = LINKADDR_FRELAY;
            break;
        default:
            linkaddr = LINKADDR_ETHER;
             break;
 	}
 
	if (!ND_TTEST2(*ar_tpa(ap), PROTO_LEN(ap))) {
 		ND_PRINT((ndo, "%s", tstr));
 		ND_DEFAULTPRINT((const u_char *)ap, length);
 		return;
	}

        if (!ndo->ndo_eflag) {
            ND_PRINT((ndo, "ARP, "));
        }

        /* print hardware type/len and proto type/len */
        if ((pro != ETHERTYPE_IP && pro != ETHERTYPE_TRAIL) ||
	    PROTO_LEN(ap) != 4 ||
            HRD_LEN(ap) == 0 ||
            ndo->ndo_vflag) {
            ND_PRINT((ndo, "%s (len %u), %s (len %u)",
                      tok2str(arphrd_values, "Unknown Hardware (%u)", hrd),
                      HRD_LEN(ap),
                      tok2str(ethertype_values, "Unknown Protocol (0x%04x)", pro),
                      PROTO_LEN(ap)));

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
		ND_PRINT((ndo, "who-has %s", ipaddr_string(ndo, TPA(ap))));
 		if (isnonzero((const u_char *)THA(ap), HRD_LEN(ap)))
 			ND_PRINT((ndo, " (%s)",
 				  linkaddr_string(ndo, THA(ap), linkaddr, HRD_LEN(ap))));
		ND_PRINT((ndo, " tell %s", ipaddr_string(ndo, SPA(ap))));
 		break;
 
 	case ARPOP_REPLY:
		ND_PRINT((ndo, "%s is-at %s",
                          ipaddr_string(ndo, SPA(ap)),
                           linkaddr_string(ndo, SHA(ap), linkaddr, HRD_LEN(ap))));
 		break;
 
	case ARPOP_REVREQUEST:
		ND_PRINT((ndo, "who-is %s tell %s",
			  linkaddr_string(ndo, THA(ap), linkaddr, HRD_LEN(ap)),
			  linkaddr_string(ndo, SHA(ap), linkaddr, HRD_LEN(ap))));
 		break;
 
 	case ARPOP_REVREPLY:
		ND_PRINT((ndo, "%s at %s",
			  linkaddr_string(ndo, THA(ap), linkaddr, HRD_LEN(ap)),
			  ipaddr_string(ndo, TPA(ap))));
 		break;
 
 	case ARPOP_INVREQUEST:
		ND_PRINT((ndo, "who-is %s tell %s",
			  linkaddr_string(ndo, THA(ap), linkaddr, HRD_LEN(ap)),
			  linkaddr_string(ndo, SHA(ap), linkaddr, HRD_LEN(ap))));
 		break;
 
 	case ARPOP_INVREPLY:
		ND_PRINT((ndo,"%s at %s",
			  linkaddr_string(ndo, SHA(ap), linkaddr, HRD_LEN(ap)),
			  ipaddr_string(ndo, SPA(ap))));
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
