mldv2_query_print(netdissect_options *ndo, const u_char *bp, u_int len)
{
    const struct icmp6_hdr *icp = (const struct icmp6_hdr *) bp;
    u_int mrc;
    int mrt, qqi;
    u_int nsrcs;
    register u_int i;

    /* Minimum len is 28 */
    if (len < 28) {
            ND_PRINT((ndo," [invalid len %d]", len));
	return;
    }
    ND_TCHECK(icp->icmp6_data16[0]);
    mrc = EXTRACT_16BITS(&icp->icmp6_data16[0]);
    if (mrc < 32768) {
	mrt = mrc;
    } else {
        mrt = ((mrc & 0x0fff) | 0x1000) << (((mrc & 0x7000) >> 12) + 3);
    }
    if (ndo->ndo_vflag) {
            ND_PRINT((ndo," [max resp delay=%d]", mrt));
    }
    ND_TCHECK2(bp[8], sizeof(struct in6_addr));
    ND_PRINT((ndo," [gaddr %s", ip6addr_string(ndo, &bp[8])));

    if (ndo->ndo_vflag) {
        ND_TCHECK(bp[25]);
	if (bp[24] & 0x08) {
		ND_PRINT((ndo," sflag"));
	}
	if (bp[24] & 0x07) {
		ND_PRINT((ndo," robustness=%d", bp[24] & 0x07));
	}
	if (bp[25] < 128) {
		qqi = bp[25];
	} else {
		qqi = ((bp[25] & 0x0f) | 0x10) << (((bp[25] & 0x70) >> 4) + 3);
	}
	ND_PRINT((ndo," qqi=%d", qqi));
    }

    ND_TCHECK2(bp[26], 2);
    nsrcs = EXTRACT_16BITS(&bp[26]);
    if (nsrcs > 0) {
	if (len < 28 + nsrcs * sizeof(struct in6_addr))
	    ND_PRINT((ndo," [invalid number of sources]"));
	else if (ndo->ndo_vflag > 1) {
	    ND_PRINT((ndo," {"));
	    for (i = 0; i < nsrcs; i++) {
		ND_TCHECK2(bp[28 + i * sizeof(struct in6_addr)],
                        sizeof(struct in6_addr));
		ND_PRINT((ndo," %s", ip6addr_string(ndo, &bp[28 + i * sizeof(struct in6_addr)])));
	    }
	    ND_PRINT((ndo," }"));
	} else
                ND_PRINT((ndo,", %d source(s)", nsrcs));
    }
     ND_PRINT((ndo,"]"));
     return;
 trunc:
    ND_PRINT((ndo,"[|icmp6]"));
     return;
 }
