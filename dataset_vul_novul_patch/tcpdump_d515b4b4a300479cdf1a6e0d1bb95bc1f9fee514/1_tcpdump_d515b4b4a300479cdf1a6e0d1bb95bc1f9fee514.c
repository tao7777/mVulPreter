decode_multicast_vpn(netdissect_options *ndo,
                     const u_char *pptr, char *buf, u_int buflen)
{
        uint8_t route_type, route_length, addr_length, sg_length;
        u_int offset;

	ND_TCHECK2(pptr[0], 2);
        route_type = *pptr++;
        route_length = *pptr++;

        snprintf(buf, buflen, "Route-Type: %s (%u), length: %u",
                 tok2str(bgp_multicast_vpn_route_type_values,
                         "Unknown", route_type),
                 route_type, route_length);

        switch(route_type) {
        case BGP_MULTICAST_VPN_ROUTE_TYPE_INTRA_AS_I_PMSI:
            ND_TCHECK2(pptr[0], BGP_VPN_RD_LEN);
            offset = strlen(buf);
            snprintf(buf + offset, buflen - offset, ", RD: %s, Originator %s",
                     bgp_vpn_rd_print(ndo, pptr),
                     bgp_vpn_ip_print(ndo, pptr + BGP_VPN_RD_LEN,
                                      (route_length - BGP_VPN_RD_LEN) << 3));
            break;
        case BGP_MULTICAST_VPN_ROUTE_TYPE_INTER_AS_I_PMSI:
            ND_TCHECK2(pptr[0], BGP_VPN_RD_LEN + 4);
            offset = strlen(buf);
	    snprintf(buf + offset, buflen - offset, ", RD: %s, Source-AS %s",
		bgp_vpn_rd_print(ndo, pptr),
		as_printf(ndo, astostr, sizeof(astostr),
		EXTRACT_32BITS(pptr + BGP_VPN_RD_LEN)));
            break;

        case BGP_MULTICAST_VPN_ROUTE_TYPE_S_PMSI:
            ND_TCHECK2(pptr[0], BGP_VPN_RD_LEN);
            offset = strlen(buf);
            snprintf(buf + offset, buflen - offset, ", RD: %s",
                     bgp_vpn_rd_print(ndo, pptr));
            pptr += BGP_VPN_RD_LEN;

            sg_length = bgp_vpn_sg_print(ndo, pptr, buf, buflen);
            addr_length =  route_length - sg_length;

            ND_TCHECK2(pptr[0], addr_length);
            offset = strlen(buf);
            snprintf(buf + offset, buflen - offset, ", Originator %s",
                     bgp_vpn_ip_print(ndo, pptr, addr_length << 3));
            break;

        case BGP_MULTICAST_VPN_ROUTE_TYPE_SOURCE_ACTIVE:
            ND_TCHECK2(pptr[0], BGP_VPN_RD_LEN);
            offset = strlen(buf);
            snprintf(buf + offset, buflen - offset, ", RD: %s",
                     bgp_vpn_rd_print(ndo, pptr));
            pptr += BGP_VPN_RD_LEN;

            bgp_vpn_sg_print(ndo, pptr, buf, buflen);
            break;
 
         case BGP_MULTICAST_VPN_ROUTE_TYPE_SHARED_TREE_JOIN: /* fall through */
         case BGP_MULTICAST_VPN_ROUTE_TYPE_SOURCE_TREE_JOIN:
            ND_TCHECK2(pptr[0], BGP_VPN_RD_LEN);
             offset = strlen(buf);
 	    snprintf(buf + offset, buflen - offset, ", RD: %s, Source-AS %s",
 		bgp_vpn_rd_print(ndo, pptr),
 		as_printf(ndo, astostr, sizeof(astostr),
 		EXTRACT_32BITS(pptr + BGP_VPN_RD_LEN)));
            pptr += BGP_VPN_RD_LEN;
 
             bgp_vpn_sg_print(ndo, pptr, buf, buflen);
             break;

            /*
             * no per route-type printing yet.
             */
        case BGP_MULTICAST_VPN_ROUTE_TYPE_INTRA_AS_SEG_LEAF:
        default:
            break;
        }

        return route_length + 2;

trunc:
	return -2;
}
