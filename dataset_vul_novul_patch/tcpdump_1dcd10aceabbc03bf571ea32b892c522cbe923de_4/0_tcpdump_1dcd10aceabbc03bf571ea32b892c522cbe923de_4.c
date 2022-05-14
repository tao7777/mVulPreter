isoclns_print(netdissect_options *ndo,
isoclns_print(netdissect_options *ndo, const uint8_t *p, u_int length)
 {
	if (!ND_TTEST(*p)) { /* enough bytes on the wire ? */
 		ND_PRINT((ndo, "|OSI"));
 		return;
 	}

	if (ndo->ndo_eflag)
		ND_PRINT((ndo, "OSI NLPID %s (0x%02x): ", tok2str(nlpid_values, "Unknown", *p), *p));

	switch (*p) {
 
 	case NLPID_CLNP:
 		if (!clnp_print(ndo, p, length))
			print_unknown_data(ndo, p, "\n\t", length);
 		break;
 
 	case NLPID_ESIS:
		esis_print(ndo, p, length);
		return;
 
 	case NLPID_ISIS:
 		if (!isis_print(ndo, p, length))
			print_unknown_data(ndo, p, "\n\t", length);
 		break;
 
 	case NLPID_NULLNS:
		ND_PRINT((ndo, "%slength: %u", ndo->ndo_eflag ? "" : ", ", length));
		break;

	case NLPID_Q933:
		q933_print(ndo, p + 1, length - 1);
		break;

	case NLPID_IP:
		ip_print(ndo, p + 1, length - 1);
		break;

	case NLPID_IP6:
		ip6_print(ndo, p + 1, length - 1);
		break;

	case NLPID_PPP:
		ppp_print(ndo, p + 1, length - 1);
		break;

	default:
 		if (!ndo->ndo_eflag)
 			ND_PRINT((ndo, "OSI NLPID 0x%02x unknown", *p));
 		ND_PRINT((ndo, "%slength: %u", ndo->ndo_eflag ? "" : ", ", length));
		if (length > 1)
			print_unknown_data(ndo, p, "\n\t", length);
 		break;
 	}
 }
