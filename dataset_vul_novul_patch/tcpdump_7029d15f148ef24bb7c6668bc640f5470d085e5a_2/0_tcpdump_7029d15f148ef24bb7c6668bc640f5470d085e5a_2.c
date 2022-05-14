print_lcp_config_options(netdissect_options *ndo,
                         const u_char *p, int length)
{
	int len, opt;

	if (length < 2)
		return 0;
	ND_TCHECK2(*p, 2);
	len = p[1];
	opt = p[0];
	if (length < len)
		return 0;
	if (len < 2) {
		if ((opt >= LCPOPT_MIN) && (opt <= LCPOPT_MAX))
			ND_PRINT((ndo, "\n\t  %s Option (0x%02x), length %u (length bogus, should be >= 2)",
			          lcpconfopts[opt], opt, len));
		else
			ND_PRINT((ndo, "\n\tunknown LCP option 0x%02x", opt));
		return 0;
	}
	if ((opt >= LCPOPT_MIN) && (opt <= LCPOPT_MAX))
		ND_PRINT((ndo, "\n\t  %s Option (0x%02x), length %u", lcpconfopts[opt], opt, len));
	else {
		ND_PRINT((ndo, "\n\tunknown LCP option 0x%02x", opt));
		return len;
	}

	switch (opt) {
	case LCPOPT_VEXT:
		if (len < 6) {
 			ND_PRINT((ndo, " (length bogus, should be >= 6)"));
 			return len;
 		}
		ND_TCHECK_24BITS(p + 2);
 		ND_PRINT((ndo, ": Vendor: %s (%u)",
 			tok2str(oui_values,"Unknown",EXTRACT_24BITS(p+2)),
 			EXTRACT_24BITS(p + 2)));
#if 0
		ND_TCHECK(p[5]);
		ND_PRINT((ndo, ", kind: 0x%02x", p[5]));
		ND_PRINT((ndo, ", Value: 0x"));
		for (i = 0; i < len - 6; i++) {
			ND_TCHECK(p[6 + i]);
			ND_PRINT((ndo, "%02x", p[6 + i]));
		}
#endif
		break;
	case LCPOPT_MRU:
		if (len != 4) {
 			ND_PRINT((ndo, " (length bogus, should be = 4)"));
 			return len;
 		}
		ND_TCHECK_16BITS(p + 2);
 		ND_PRINT((ndo, ": %u", EXTRACT_16BITS(p + 2)));
 		break;
 	case LCPOPT_ACCM:
 		if (len != 6) {
 			ND_PRINT((ndo, " (length bogus, should be = 6)"));
 			return len;
 		}
		ND_TCHECK_32BITS(p + 2);
 		ND_PRINT((ndo, ": 0x%08x", EXTRACT_32BITS(p + 2)));
 		break;
 	case LCPOPT_AP:
 		if (len < 4) {
 			ND_PRINT((ndo, " (length bogus, should be >= 4)"));
 			return len;
 		}
		ND_TCHECK_16BITS(p + 2);
 		ND_PRINT((ndo, ": %s", tok2str(ppptype2str, "Unknown Auth Proto (0x04x)", EXTRACT_16BITS(p + 2))));
 
 		switch (EXTRACT_16BITS(p+2)) {
		case PPP_CHAP:
			ND_TCHECK(p[4]);
			ND_PRINT((ndo, ", %s", tok2str(authalg_values, "Unknown Auth Alg %u", p[4])));
			break;
		case PPP_PAP: /* fall through */
		case PPP_EAP:
		case PPP_SPAP:
		case PPP_SPAP_OLD:
                        break;
		default:
			print_unknown_data(ndo, p, "\n\t", len);
		}
		break;
	case LCPOPT_QP:
		if (len < 4) {
 			ND_PRINT((ndo, " (length bogus, should be >= 4)"));
 			return 0;
 		}
		ND_TCHECK_16BITS(p+2);
 		if (EXTRACT_16BITS(p+2) == PPP_LQM)
 			ND_PRINT((ndo, ": LQR"));
 		else
			ND_PRINT((ndo, ": unknown"));
		break;
	case LCPOPT_MN:
		if (len != 6) {
 			ND_PRINT((ndo, " (length bogus, should be = 6)"));
 			return 0;
 		}
		ND_TCHECK_32BITS(p + 2);
 		ND_PRINT((ndo, ": 0x%08x", EXTRACT_32BITS(p + 2)));
 		break;
 	case LCPOPT_PFC:
		break;
	case LCPOPT_ACFC:
		break;
	case LCPOPT_LD:
		if (len != 4) {
 			ND_PRINT((ndo, " (length bogus, should be = 4)"));
 			return 0;
 		}
		ND_TCHECK_16BITS(p + 2);
 		ND_PRINT((ndo, ": 0x%04x", EXTRACT_16BITS(p + 2)));
 		break;
 	case LCPOPT_CBACK:
		if (len < 3) {
			ND_PRINT((ndo, " (length bogus, should be >= 3)"));
			return 0;
		}
		ND_PRINT((ndo, ": "));
		ND_TCHECK(p[2]);
		ND_PRINT((ndo, ": Callback Operation %s (%u)",
                       tok2str(ppp_callback_values, "Unknown", p[2]),
                       p[2]));
		break;
	case LCPOPT_MLMRRU:
		if (len != 4) {
 			ND_PRINT((ndo, " (length bogus, should be = 4)"));
 			return 0;
 		}
		ND_TCHECK_16BITS(p + 2);
 		ND_PRINT((ndo, ": %u", EXTRACT_16BITS(p + 2)));
 		break;
 	case LCPOPT_MLED:
		if (len < 3) {
			ND_PRINT((ndo, " (length bogus, should be >= 3)"));
			return 0;
		}
		ND_TCHECK(p[2]);
		switch (p[2]) {		/* class */
		case MEDCLASS_NULL:
			ND_PRINT((ndo, ": Null"));
			break;
		case MEDCLASS_LOCAL:
			ND_PRINT((ndo, ": Local")); /* XXX */
			break;
		case MEDCLASS_IPV4:
			if (len != 7) {
				ND_PRINT((ndo, " (length bogus, should be = 7)"));
				return 0;
			}
			ND_TCHECK2(*(p + 3), 4);
			ND_PRINT((ndo, ": IPv4 %s", ipaddr_string(ndo, p + 3)));
			break;
		case MEDCLASS_MAC:
			if (len != 9) {
				ND_PRINT((ndo, " (length bogus, should be = 9)"));
				return 0;
			}
			ND_TCHECK2(*(p + 3), 6);
			ND_PRINT((ndo, ": MAC %s", etheraddr_string(ndo, p + 3)));
			break;
		case MEDCLASS_MNB:
			ND_PRINT((ndo, ": Magic-Num-Block")); /* XXX */
			break;
		case MEDCLASS_PSNDN:
			ND_PRINT((ndo, ": PSNDN")); /* XXX */
			break;
		default:
			ND_PRINT((ndo, ": Unknown class %u", p[2]));
			break;
		}
		break;

/* XXX: to be supported */
#if 0
	case LCPOPT_DEP6:
	case LCPOPT_FCSALT:
	case LCPOPT_SDP:
	case LCPOPT_NUMMODE:
	case LCPOPT_DEP12:
	case LCPOPT_DEP14:
	case LCPOPT_DEP15:
	case LCPOPT_DEP16:
        case LCPOPT_MLSSNHF:
	case LCPOPT_PROP:
	case LCPOPT_DCEID:
	case LCPOPT_MPP:
	case LCPOPT_LCPAOPT:
	case LCPOPT_COBS:
	case LCPOPT_PE:
	case LCPOPT_MLHF:
	case LCPOPT_I18N:
	case LCPOPT_SDLOS:
	case LCPOPT_PPPMUX:
		break;
#endif
	default:
		/*
		 * Unknown option; dump it as raw bytes now if we're
		 * not going to do so below.
		 */
		if (ndo->ndo_vflag < 2)
			print_unknown_data(ndo, &p[2], "\n\t    ", len - 2);
		break;
	}

	if (ndo->ndo_vflag > 1)
		print_unknown_data(ndo, &p[2], "\n\t    ", len - 2); /* exclude TLV header */

	return len;

trunc:
	ND_PRINT((ndo, "[|lcp]"));
	return 0;
}
