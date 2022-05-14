decode_rt_routing_info(netdissect_options *ndo,
                       const u_char *pptr, char *buf, u_int buflen)
 {
 	uint8_t route_target[8];
 	u_int plen;
 
 	ND_TCHECK(pptr[0]);
 	plen = pptr[0];   /* get prefix length */
 
 	if (0 == plen) {
 		snprintf(buf, buflen, "default route target");
 		return 1;
 	}
 
 	if (32 > plen)
 		return -1;
 
         plen-=32; /* adjust prefix length */
 
 	if (64 < plen)
 		return -1;
 
 	memset(&route_target, 0, sizeof(route_target));
	ND_TCHECK2(pptr[1], (plen + 7) / 8);
	memcpy(&route_target, &pptr[1], (plen + 7) / 8);
 	if (plen % 8) {
 		((u_char *)&route_target)[(plen + 7) / 8 - 1] &=
 			((0xff00 >> (plen % 8)) & 0xff);
 	}
 	snprintf(buf, buflen, "origin AS: %s, route target %s",
	    as_printf(ndo, astostr, sizeof(astostr), EXTRACT_32BITS(pptr+1)),
 	    bgp_vpn_rd_print(ndo, (u_char *)&route_target));
 
 	return 5 + (plen + 7) / 8;

trunc:
	return -2;
}
