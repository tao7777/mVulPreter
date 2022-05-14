xid_map_enter(netdissect_options *ndo,
              const struct sunrpc_msg *rp, const u_char *bp)
{
	const struct ip *ip = NULL;
	const struct ip6_hdr *ip6 = NULL;
	struct xid_map_entry *xmep;

	if (!ND_TTEST(rp->rm_call.cb_vers))
		return (0);
	switch (IP_V((const struct ip *)bp)) {
	case 4:
		ip = (const struct ip *)bp;
		break;
	case 6:
		ip6 = (const struct ip6_hdr *)bp;
		break;
	default:
		return (1);
	}

	xmep = &xid_map[xid_map_next];

	if (++xid_map_next >= XIDMAPSIZE)
		xid_map_next = 0;

	UNALIGNED_MEMCPY(&xmep->xid, &rp->rm_xid, sizeof(xmep->xid));
	if (ip) {
		xmep->ipver = 4;
		UNALIGNED_MEMCPY(&xmep->client, &ip->ip_src, sizeof(ip->ip_src));
		UNALIGNED_MEMCPY(&xmep->server, &ip->ip_dst, sizeof(ip->ip_dst));
	}
	else if (ip6) {
		xmep->ipver = 6;
 		UNALIGNED_MEMCPY(&xmep->client, &ip6->ip6_src, sizeof(ip6->ip6_src));
 		UNALIGNED_MEMCPY(&xmep->server, &ip6->ip6_dst, sizeof(ip6->ip6_dst));
 	}
	if (!ND_TTEST(rp->rm_call.cb_proc))
		return (0);
 	xmep->proc = EXTRACT_32BITS(&rp->rm_call.cb_proc);
	if (!ND_TTEST(rp->rm_call.cb_vers))
		return (0);
 	xmep->vers = EXTRACT_32BITS(&rp->rm_call.cb_vers);
 	return (1);
 }
