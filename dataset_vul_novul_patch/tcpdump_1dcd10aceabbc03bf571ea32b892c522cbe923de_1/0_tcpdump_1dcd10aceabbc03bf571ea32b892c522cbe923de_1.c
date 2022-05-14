ethertype_print(netdissect_options *ndo,
                u_short ether_type, const u_char *p,
                u_int length, u_int caplen,
                const struct lladdr_info *src, const struct lladdr_info *dst)
{
	switch (ether_type) {

	case ETHERTYPE_IP:
	        ip_print(ndo, p, length);
		return (1);

	case ETHERTYPE_IPV6:
		ip6_print(ndo, p, length);
		return (1);

	case ETHERTYPE_ARP:
	case ETHERTYPE_REVARP:
	        arp_print(ndo, p, length, caplen);
		return (1);

	case ETHERTYPE_DN:
		decnet_print(ndo, p, length, caplen);
		return (1);

	case ETHERTYPE_ATALK:
		if (ndo->ndo_vflag)
			ND_PRINT((ndo, "et1 "));
		atalk_print(ndo, p, length);
		return (1);

	case ETHERTYPE_AARP:
		aarp_print(ndo, p, length);
		return (1);

	case ETHERTYPE_IPX:
		ND_PRINT((ndo, "(NOV-ETHII) "));
		ipx_print(ndo, p, length);
		return (1);

	case ETHERTYPE_ISO:
		if (length == 0 || caplen == 0) {
 			ND_PRINT((ndo, " [|osi]"));
 			return (1);
 		}
		isoclns_print(ndo, p + 1, length - 1);
 		return(1);
 
 	case ETHERTYPE_PPPOED:
	case ETHERTYPE_PPPOES:
	case ETHERTYPE_PPPOED2:
	case ETHERTYPE_PPPOES2:
		pppoe_print(ndo, p, length);
		return (1);

	case ETHERTYPE_EAPOL:
	        eap_print(ndo, p, length);
		return (1);

	case ETHERTYPE_RRCP:
	        rrcp_print(ndo, p, length, src, dst);
		return (1);

	case ETHERTYPE_PPP:
		if (length) {
			ND_PRINT((ndo, ": "));
			ppp_print(ndo, p, length);
		}
		return (1);

	case ETHERTYPE_MPCP:
	        mpcp_print(ndo, p, length);
		return (1);

	case ETHERTYPE_SLOW:
	        slow_print(ndo, p, length);
		return (1);

	case ETHERTYPE_CFM:
	case ETHERTYPE_CFM_OLD:
		cfm_print(ndo, p, length);
		return (1);

	case ETHERTYPE_LLDP:
		lldp_print(ndo, p, length);
		return (1);

        case ETHERTYPE_NSH:
                nsh_print(ndo, p, length);
                return (1);

        case ETHERTYPE_LOOPBACK:
		loopback_print(ndo, p, length);
                return (1);

	case ETHERTYPE_MPLS:
	case ETHERTYPE_MPLS_MULTI:
		mpls_print(ndo, p, length);
		return (1);

	case ETHERTYPE_TIPC:
		tipc_print(ndo, p, length, caplen);
		return (1);

	case ETHERTYPE_MS_NLB_HB:
		msnlb_print(ndo, p);
		return (1);

        case ETHERTYPE_GEONET_OLD:
        case ETHERTYPE_GEONET:
                geonet_print(ndo, p, length, src);
                return (1);

        case ETHERTYPE_CALM_FAST:
                calm_fast_print(ndo, p, length, src);
                return (1);

	case ETHERTYPE_AOE:
		aoe_print(ndo, p, length);
		return (1);

	case ETHERTYPE_MEDSA:
		medsa_print(ndo, p, length, caplen, src, dst);
		return (1);

	case ETHERTYPE_LAT:
	case ETHERTYPE_SCA:
	case ETHERTYPE_MOPRC:
	case ETHERTYPE_MOPDL:
	case ETHERTYPE_IEEE1905_1:
		/* default_print for now */
	default:
		return (0);
	}
}
