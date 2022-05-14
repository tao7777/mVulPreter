ripng_print(netdissect_options *ndo, const u_char *dat, unsigned int length)
 {
 	register const struct rip6 *rp = (const struct rip6 *)dat;
 	register const struct netinfo6 *ni;
	unsigned int length_left;
	u_int j;
 
	ND_TCHECK(rp->rip6_cmd);
 	switch (rp->rip6_cmd) {
 
 	case RIP6_REQUEST:
		length_left = length;
		if (length_left < (sizeof(struct rip6) - sizeof(struct netinfo6)))
			goto trunc;
		length_left -= (sizeof(struct rip6) - sizeof(struct netinfo6));
 		j = length_left / sizeof(*ni);
		if (j == 1) {
			ND_TCHECK(rp->rip6_nets);
			if (rp->rip6_nets->rip6_metric == HOPCNT_INFINITY6
			    &&  IN6_IS_ADDR_UNSPECIFIED(&rp->rip6_nets->rip6_dest)) {
				ND_PRINT((ndo, " ripng-req dump"));
				break;
			}
 		}
		if (j * sizeof(*ni) != length_left)
			ND_PRINT((ndo, " ripng-req %u[%u]:", j, length));
 		else
			ND_PRINT((ndo, " ripng-req %u:", j));
		for (ni = rp->rip6_nets; length_left >= sizeof(*ni);
		    length_left -= sizeof(*ni), ++ni) {
			ND_TCHECK(*ni);
 			if (ndo->ndo_vflag > 1)
 				ND_PRINT((ndo, "\n\t"));
 			else
 				ND_PRINT((ndo, " "));
 			rip6_entry_print(ndo, ni, 0);
 		}
		if (length_left != 0)
			goto trunc;
 		break;
 	case RIP6_RESPONSE:
		length_left = length;
		if (length_left < (sizeof(struct rip6) - sizeof(struct netinfo6)))
			goto trunc;
		length_left -= (sizeof(struct rip6) - sizeof(struct netinfo6));
		j = length_left / sizeof(*ni);
		if (j * sizeof(*ni) != length_left)
 			ND_PRINT((ndo, " ripng-resp %d[%u]:", j, length));
 		else
 			ND_PRINT((ndo, " ripng-resp %d:", j));
		for (ni = rp->rip6_nets; length_left >= sizeof(*ni);
		    length_left -= sizeof(*ni), ++ni) {
			ND_TCHECK(*ni);
 			if (ndo->ndo_vflag > 1)
 				ND_PRINT((ndo, "\n\t"));
 			else
 				ND_PRINT((ndo, " "));
 			rip6_entry_print(ndo, ni, ni->rip6_metric);
 		}
		if (length_left != 0)
			goto trunc;
 		break;
 	default:
 		ND_PRINT((ndo, " ripng-%d ?? %u", rp->rip6_cmd, length));
 		break;
 	}
	ND_TCHECK(rp->rip6_vers);
 	if (rp->rip6_vers != RIP6_VERSION)
 		ND_PRINT((ndo, " [vers %d]", rp->rip6_vers));
	return;

trunc:
	ND_PRINT((ndo, "[|ripng]"));
	return;
 }
