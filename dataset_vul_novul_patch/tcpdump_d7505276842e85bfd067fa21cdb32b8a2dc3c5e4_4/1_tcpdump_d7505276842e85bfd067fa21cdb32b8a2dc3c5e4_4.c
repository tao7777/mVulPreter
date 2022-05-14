mldv2_report_print(netdissect_options *ndo, const u_char *bp, u_int len)
{
    const struct icmp6_hdr *icp = (const struct icmp6_hdr *) bp;
    u_int group, nsrcs, ngroups;
    u_int i, j;

    /* Minimum len is 8 */
    if (len < 8) {
            ND_PRINT((ndo," [invalid len %d]", len));
            return;
    }

    ND_TCHECK(icp->icmp6_data16[1]);
    ngroups = EXTRACT_16BITS(&icp->icmp6_data16[1]);
    ND_PRINT((ndo,", %d group record(s)", ngroups));
    if (ndo->ndo_vflag > 0) {
	/* Print the group records */
	group = 8;
        for (i = 0; i < ngroups; i++) {
	    /* type(1) + auxlen(1) + numsrc(2) + grp(16) */
	    if (len < group + 20) {
                    ND_PRINT((ndo," [invalid number of groups]"));
                    return;
	    }
            ND_TCHECK2(bp[group + 4], sizeof(struct in6_addr));
            ND_PRINT((ndo," [gaddr %s", ip6addr_string(ndo, &bp[group + 4])));
	    ND_PRINT((ndo," %s", tok2str(mldv2report2str, " [v2-report-#%d]",
                                         bp[group])));
            nsrcs = (bp[group + 2] << 8) + bp[group + 3];
	    /* Check the number of sources and print them */
	    if (len < group + 20 + (nsrcs * sizeof(struct in6_addr))) {
                    ND_PRINT((ndo," [invalid number of sources %d]", nsrcs));
                    return;
	    }
            if (ndo->ndo_vflag == 1)
                    ND_PRINT((ndo,", %d source(s)", nsrcs));
            else {
		/* Print the sources */
                    ND_PRINT((ndo," {"));
                for (j = 0; j < nsrcs; j++) {
                    ND_TCHECK2(bp[group + 20 + j * sizeof(struct in6_addr)],
                            sizeof(struct in6_addr));
		    ND_PRINT((ndo," %s", ip6addr_string(ndo, &bp[group + 20 + j * sizeof(struct in6_addr)])));
		}
                ND_PRINT((ndo," }"));
            }
	    /* Next group record */
            group += 20 + nsrcs * sizeof(struct in6_addr);
	    ND_PRINT((ndo,"]"));
        }
     }
     return;
 trunc:
    ND_PRINT((ndo,"[|icmp6]"));
     return;
 }
