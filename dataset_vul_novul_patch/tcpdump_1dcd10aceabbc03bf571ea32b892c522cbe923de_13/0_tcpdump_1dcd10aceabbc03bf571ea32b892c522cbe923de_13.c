handle_ppp(netdissect_options *ndo,
           u_int proto, const u_char *p, int length)
{
	if ((proto & 0xff00) == 0x7e00) { /* is this an escape code ? */
		ppp_hdlc(ndo, p - 1, length);
		return;
	}

	switch (proto) {
	case PPP_LCP: /* fall through */
	case PPP_IPCP:
	case PPP_OSICP:
	case PPP_MPLSCP:
	case PPP_IPV6CP:
	case PPP_CCP:
	case PPP_BACP:
		handle_ctrl_proto(ndo, proto, p, length);
		break;
	case PPP_ML:
		handle_mlppp(ndo, p, length);
		break;
	case PPP_CHAP:
		handle_chap(ndo, p, length);
		break;
	case PPP_PAP:
		handle_pap(ndo, p, length);
		break;
	case PPP_BAP:		/* XXX: not yet completed */
		handle_bap(ndo, p, length);
		break;
	case ETHERTYPE_IP:	/*XXX*/
        case PPP_VJNC:
	case PPP_IP:
		ip_print(ndo, p, length);
		break;
	case ETHERTYPE_IPV6:	/*XXX*/
	case PPP_IPV6:
		ip6_print(ndo, p, length);
		break;
	case ETHERTYPE_IPX:	/*XXX*/
	case PPP_IPX:
 		ipx_print(ndo, p, length);
 		break;
 	case PPP_OSI:
		isoclns_print(ndo, p, length);
 		break;
 	case PPP_MPLS_UCAST:
 	case PPP_MPLS_MCAST:
		mpls_print(ndo, p, length);
		break;
	case PPP_COMP:
		ND_PRINT((ndo, "compressed PPP data"));
		break;
	default:
		ND_PRINT((ndo, "%s ", tok2str(ppptype2str, "unknown PPP protocol (0x%04x)", proto)));
		print_unknown_data(ndo, p, "\n\t", length);
		break;
	}
}
