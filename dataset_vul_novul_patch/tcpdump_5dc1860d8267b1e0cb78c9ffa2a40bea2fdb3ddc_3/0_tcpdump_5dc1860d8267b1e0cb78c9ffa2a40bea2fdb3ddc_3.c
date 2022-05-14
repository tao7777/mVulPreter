 pimv2_addr_print(netdissect_options *ndo,
                 const u_char *bp, u_int len, enum pimv2_addrtype at,
                 u_int addr_len, int silent)
 {
 	int af;
	int hdrlen;
 
	if (addr_len == 0) {
		if (len < 2)
			goto trunc;
 		ND_TCHECK(bp[1]);
 		switch (bp[0]) {
 		case 1:
 			af = AF_INET;
			addr_len = (u_int)sizeof(struct in_addr);
 			break;
 		case 2:
 			af = AF_INET6;
			addr_len = (u_int)sizeof(struct in6_addr);
 			break;
 		default:
 			return -1;
		}
		if (bp[1] != 0)
 			return -1;
 		hdrlen = 2;
 	} else {
		switch (addr_len) {
 		case sizeof(struct in_addr):
 			af = AF_INET;
 			break;
		case sizeof(struct in6_addr):
			af = AF_INET6;
			break;
		default:
 			return -1;
 			break;
 		}
 		hdrlen = 0;
 	}
 
 	bp += hdrlen;
	len -= hdrlen;
 	switch (at) {
 	case pimv2_unicast:
		if (len < addr_len)
			goto trunc;
		ND_TCHECK2(bp[0], addr_len);
 		if (af == AF_INET) {
 			if (!silent)
 				ND_PRINT((ndo, "%s", ipaddr_string(ndo, bp)));
		}
		else if (af == AF_INET6) {
 			if (!silent)
 				ND_PRINT((ndo, "%s", ip6addr_string(ndo, bp)));
 		}
		return hdrlen + addr_len;
 	case pimv2_group:
 	case pimv2_source:
		if (len < addr_len + 2)
			goto trunc;
		ND_TCHECK2(bp[0], addr_len + 2);
 		if (af == AF_INET) {
 			if (!silent) {
 				ND_PRINT((ndo, "%s", ipaddr_string(ndo, bp + 2)));
				if (bp[1] != 32)
					ND_PRINT((ndo, "/%u", bp[1]));
			}
		}
		else if (af == AF_INET6) {
			if (!silent) {
				ND_PRINT((ndo, "%s", ip6addr_string(ndo, bp + 2)));
				if (bp[1] != 128)
					ND_PRINT((ndo, "/%u", bp[1]));
			}
		}
		if (bp[0] && !silent) {
			if (at == pimv2_group) {
				ND_PRINT((ndo, "(0x%02x)", bp[0]));
			} else {
				ND_PRINT((ndo, "(%s%s%s",
					bp[0] & 0x04 ? "S" : "",
					bp[0] & 0x02 ? "W" : "",
					bp[0] & 0x01 ? "R" : ""));
				if (bp[0] & 0xf8) {
					ND_PRINT((ndo, "+0x%02x", bp[0] & 0xf8));
				}
 				ND_PRINT((ndo, ")"));
 			}
 		}
		return hdrlen + 2 + addr_len;
 	default:
 		return -1;
 	}
trunc:
	return -1;
}
