 pimv2_addr_print(netdissect_options *ndo,
                 const u_char *bp, enum pimv2_addrtype at, int silent)
 {
 	int af;
	int len, hdrlen;
 
	ND_TCHECK(bp[0]);
	if (pimv2_addr_len == 0) {
 		ND_TCHECK(bp[1]);
 		switch (bp[0]) {
 		case 1:
 			af = AF_INET;
			len = sizeof(struct in_addr);
 			break;
 		case 2:
 			af = AF_INET6;
			len = sizeof(struct in6_addr);
 			break;
 		default:
 			return -1;
		}
		if (bp[1] != 0)
 			return -1;
 		hdrlen = 2;
 	} else {
		switch (pimv2_addr_len) {
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
		len = pimv2_addr_len;
 		hdrlen = 0;
 	}
 
 	bp += hdrlen;
 	switch (at) {
 	case pimv2_unicast:
		ND_TCHECK2(bp[0], len);
 		if (af == AF_INET) {
 			if (!silent)
 				ND_PRINT((ndo, "%s", ipaddr_string(ndo, bp)));
		}
		else if (af == AF_INET6) {
 			if (!silent)
 				ND_PRINT((ndo, "%s", ip6addr_string(ndo, bp)));
 		}
		return hdrlen + len;
 	case pimv2_group:
 	case pimv2_source:
		ND_TCHECK2(bp[0], len + 2);
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
		return hdrlen + 2 + len;
 	default:
 		return -1;
 	}
trunc:
	return -1;
}
