icmp6_print(netdissect_options *ndo,
            const u_char *bp, u_int length, const u_char *bp2, int fragmented)
{
	const struct icmp6_hdr *dp;
	const struct ip6_hdr *ip;
	const struct ip6_hdr *oip;
	const struct udphdr *ouh;
	int dport;
	const u_char *ep;
	u_int prot;

	dp = (const struct icmp6_hdr *)bp;
	ip = (const struct ip6_hdr *)bp2;
	oip = (const struct ip6_hdr *)(dp + 1);
	/* 'ep' points to the end of available data. */
	ep = ndo->ndo_snapend;

	ND_TCHECK(dp->icmp6_cksum);

	if (ndo->ndo_vflag && !fragmented) {
		uint16_t sum, udp_sum;

		if (ND_TTEST2(bp[0], length)) {
			udp_sum = EXTRACT_16BITS(&dp->icmp6_cksum);
			sum = icmp6_cksum(ndo, ip, dp, length);
			if (sum != 0)
				ND_PRINT((ndo,"[bad icmp6 cksum 0x%04x -> 0x%04x!] ",
                                                udp_sum,
                                                in_cksum_shouldbe(udp_sum, sum)));
			else
				ND_PRINT((ndo,"[icmp6 sum ok] "));
		}
	}

        ND_PRINT((ndo,"ICMP6, %s", tok2str(icmp6_type_values,"unknown icmp6 type (%u)",dp->icmp6_type)));

        /* display cosmetics: print the packet length for printer that use the vflag now */
        if (ndo->ndo_vflag && (dp->icmp6_type == ND_ROUTER_SOLICIT ||
                      dp->icmp6_type == ND_ROUTER_ADVERT ||
                      dp->icmp6_type == ND_NEIGHBOR_ADVERT ||
                      dp->icmp6_type == ND_NEIGHBOR_SOLICIT ||
                      dp->icmp6_type == ND_REDIRECT ||
                      dp->icmp6_type == ICMP6_HADISCOV_REPLY ||
                      dp->icmp6_type == ICMP6_MOBILEPREFIX_ADVERT ))
                ND_PRINT((ndo,", length %u", length));

	switch (dp->icmp6_type) {
	case ICMP6_DST_UNREACH:
		ND_TCHECK(oip->ip6_dst);
                ND_PRINT((ndo,", %s", tok2str(icmp6_dst_unreach_code_values,"unknown unreach code (%u)",dp->icmp6_code)));
		switch (dp->icmp6_code) {

		case ICMP6_DST_UNREACH_NOROUTE: /* fall through */
		case ICMP6_DST_UNREACH_ADMIN:
		case ICMP6_DST_UNREACH_ADDR:
                        ND_PRINT((ndo," %s",ip6addr_string(ndo, &oip->ip6_dst)));
                        break;
		case ICMP6_DST_UNREACH_BEYONDSCOPE:
			ND_PRINT((ndo," %s, source address %s",
			       ip6addr_string(ndo, &oip->ip6_dst),
                                  ip6addr_string(ndo, &oip->ip6_src)));
			break;
		case ICMP6_DST_UNREACH_NOPORT:
			if ((ouh = get_upperlayer(ndo, (const u_char *)oip, &prot))
			    == NULL)
				goto trunc;

			dport = EXTRACT_16BITS(&ouh->uh_dport);
			switch (prot) {
			case IPPROTO_TCP:
				ND_PRINT((ndo,", %s tcp port %s",
					ip6addr_string(ndo, &oip->ip6_dst),
                                          tcpport_string(ndo, dport)));
				break;
			case IPPROTO_UDP:
				ND_PRINT((ndo,", %s udp port %s",
					ip6addr_string(ndo, &oip->ip6_dst),
                                          udpport_string(ndo, dport)));
				break;
			default:
				ND_PRINT((ndo,", %s protocol %d port %d unreachable",
					ip6addr_string(ndo, &oip->ip6_dst),
                                          oip->ip6_nxt, dport));
				break;
			}
			break;
		default:
                  if (ndo->ndo_vflag <= 1) {
                    print_unknown_data(ndo, bp,"\n\t",length);
                    return;
                  }
                    break;
		}
		break;
	case ICMP6_PACKET_TOO_BIG:
		ND_TCHECK(dp->icmp6_mtu);
		ND_PRINT((ndo,", mtu %u", EXTRACT_32BITS(&dp->icmp6_mtu)));
		break;
	case ICMP6_TIME_EXCEEDED:
		ND_TCHECK(oip->ip6_dst);
		switch (dp->icmp6_code) {
		case ICMP6_TIME_EXCEED_TRANSIT:
			ND_PRINT((ndo," for %s",
                                  ip6addr_string(ndo, &oip->ip6_dst)));
			break;
		case ICMP6_TIME_EXCEED_REASSEMBLY:
			ND_PRINT((ndo," (reassembly)"));
			break;
		default:
                        ND_PRINT((ndo,", unknown code (%u)", dp->icmp6_code));
			break;
		}
		break;
	case ICMP6_PARAM_PROB:
		ND_TCHECK(oip->ip6_dst);
		switch (dp->icmp6_code) {
		case ICMP6_PARAMPROB_HEADER:
                        ND_PRINT((ndo,", erroneous - octet %u", EXTRACT_32BITS(&dp->icmp6_pptr)));
                        break;
		case ICMP6_PARAMPROB_NEXTHEADER:
                        ND_PRINT((ndo,", next header - octet %u", EXTRACT_32BITS(&dp->icmp6_pptr)));
                        break;
		case ICMP6_PARAMPROB_OPTION:
                        ND_PRINT((ndo,", option - octet %u", EXTRACT_32BITS(&dp->icmp6_pptr)));
                        break;
		default:
                        ND_PRINT((ndo,", code-#%d",
                                  dp->icmp6_code));
                        break;
		}
		break;
	case ICMP6_ECHO_REQUEST:
	case ICMP6_ECHO_REPLY:
                ND_TCHECK(dp->icmp6_seq);
                ND_PRINT((ndo,", seq %u", EXTRACT_16BITS(&dp->icmp6_seq)));
		break;
	case ICMP6_MEMBERSHIP_QUERY:
		if (length == MLD_MINLEN) {
			mld6_print(ndo, (const u_char *)dp);
		} else if (length >= MLDV2_MINLEN) {
			ND_PRINT((ndo," v2"));
			mldv2_query_print(ndo, (const u_char *)dp, length);
		} else {
                        ND_PRINT((ndo," unknown-version (len %u) ", length));
		}
		break;
	case ICMP6_MEMBERSHIP_REPORT:
		mld6_print(ndo, (const u_char *)dp);
		break;
	case ICMP6_MEMBERSHIP_REDUCTION:
		mld6_print(ndo, (const u_char *)dp);
		break;
	case ND_ROUTER_SOLICIT:
#define RTSOLLEN 8
		if (ndo->ndo_vflag) {
			icmp6_opt_print(ndo, (const u_char *)dp + RTSOLLEN,
					length - RTSOLLEN);
		}
		break;
	case ND_ROUTER_ADVERT:
#define RTADVLEN 16
		if (ndo->ndo_vflag) {
			const struct nd_router_advert *p;

			p = (const struct nd_router_advert *)dp;
			ND_TCHECK(p->nd_ra_retransmit);
			ND_PRINT((ndo,"\n\thop limit %u, Flags [%s]" \
                                  ", pref %s, router lifetime %us, reachable time %us, retrans time %us",
                                  (u_int)p->nd_ra_curhoplimit,
                                  bittok2str(icmp6_opt_ra_flag_values,"none",(p->nd_ra_flags_reserved)),
                                  get_rtpref(p->nd_ra_flags_reserved),
                                  EXTRACT_16BITS(&p->nd_ra_router_lifetime),
                                  EXTRACT_32BITS(&p->nd_ra_reachable),
                                  EXTRACT_32BITS(&p->nd_ra_retransmit)));

			icmp6_opt_print(ndo, (const u_char *)dp + RTADVLEN,
					length - RTADVLEN);
		}
		break;
	case ND_NEIGHBOR_SOLICIT:
	    {
		const struct nd_neighbor_solicit *p;
		p = (const struct nd_neighbor_solicit *)dp;
		ND_TCHECK(p->nd_ns_target);
		ND_PRINT((ndo,", who has %s", ip6addr_string(ndo, &p->nd_ns_target)));
		if (ndo->ndo_vflag) {
#define NDSOLLEN 24
			icmp6_opt_print(ndo, (const u_char *)dp + NDSOLLEN,
					length - NDSOLLEN);
		}
	    }
		break;
	case ND_NEIGHBOR_ADVERT:
	    {
		const struct nd_neighbor_advert *p;

		p = (const struct nd_neighbor_advert *)dp;
		ND_TCHECK(p->nd_na_target);
		ND_PRINT((ndo,", tgt is %s",
                          ip6addr_string(ndo, &p->nd_na_target)));
		if (ndo->ndo_vflag) {
                        ND_PRINT((ndo,", Flags [%s]",
                                  bittok2str(icmp6_nd_na_flag_values,
                                             "none",
                                             EXTRACT_32BITS(&p->nd_na_flags_reserved))));
#define NDADVLEN 24
			icmp6_opt_print(ndo, (const u_char *)dp + NDADVLEN,
					length - NDADVLEN);
#undef NDADVLEN
		}
	    }
		break;
	case ND_REDIRECT:
#define RDR(i) ((const struct nd_redirect *)(i))
                         ND_TCHECK(RDR(dp)->nd_rd_dst);
                         ND_PRINT((ndo,", %s", ip6addr_string(ndo, &RDR(dp)->nd_rd_dst)));
		ND_TCHECK(RDR(dp)->nd_rd_target);
		ND_PRINT((ndo," to %s",
                          ip6addr_string(ndo, &RDR(dp)->nd_rd_target)));
#define REDIRECTLEN 40
		if (ndo->ndo_vflag) {
			icmp6_opt_print(ndo, (const u_char *)dp + REDIRECTLEN,
					length - REDIRECTLEN);
		}
		break;
#undef REDIRECTLEN
#undef RDR
	case ICMP6_ROUTER_RENUMBERING:
		icmp6_rrenum_print(ndo, bp, ep);
		break;
	case ICMP6_NI_QUERY:
	case ICMP6_NI_REPLY:
		icmp6_nodeinfo_print(ndo, length, bp, ep);
		break;
	case IND_SOLICIT:
	case IND_ADVERT:
		break;
	case ICMP6_V2_MEMBERSHIP_REPORT:
		mldv2_report_print(ndo, (const u_char *) dp, length);
		break;
	case ICMP6_MOBILEPREFIX_SOLICIT: /* fall through */
	case ICMP6_HADISCOV_REQUEST:
                ND_TCHECK(dp->icmp6_data16[0]);
                ND_PRINT((ndo,", id 0x%04x", EXTRACT_16BITS(&dp->icmp6_data16[0])));
                break;
	case ICMP6_HADISCOV_REPLY:
		if (ndo->ndo_vflag) {
			const struct in6_addr *in6;
			const u_char *cp;

			ND_TCHECK(dp->icmp6_data16[0]);
			ND_PRINT((ndo,", id 0x%04x", EXTRACT_16BITS(&dp->icmp6_data16[0])));
			cp = (const u_char *)dp + length;
			in6 = (const struct in6_addr *)(dp + 1);
			for (; (const u_char *)in6 < cp; in6++) {
				ND_TCHECK(*in6);
				ND_PRINT((ndo,", %s", ip6addr_string(ndo, in6)));
			}
		}
		break;
	case ICMP6_MOBILEPREFIX_ADVERT:
 		if (ndo->ndo_vflag) {
 			ND_TCHECK(dp->icmp6_data16[0]);
 			ND_PRINT((ndo,", id 0x%04x", EXTRACT_16BITS(&dp->icmp6_data16[0])));
 			if (dp->icmp6_data16[1] & 0xc0)
 				ND_PRINT((ndo," "));
 			if (dp->icmp6_data16[1] & 0x80)
				ND_PRINT((ndo,"M"));
			if (dp->icmp6_data16[1] & 0x40)
				ND_PRINT((ndo,"O"));
#define MPADVLEN 8
			icmp6_opt_print(ndo, (const u_char *)dp + MPADVLEN,
					length - MPADVLEN);
		}
		break;
        case ND_RPL_MESSAGE:
                /* plus 4, because struct icmp6_hdr contains 4 bytes of icmp payload */
                rpl_print(ndo, dp, &dp->icmp6_data8[0], length-sizeof(struct icmp6_hdr)+4);
                break;
	default:
                ND_PRINT((ndo,", length %u", length));
                if (ndo->ndo_vflag <= 1)
                        print_unknown_data(ndo, bp,"\n\t", length);
                return;
        }
        if (!ndo->ndo_vflag)
                ND_PRINT((ndo,", length %u", length));
	return;
trunc:
	ND_PRINT((ndo, "[|icmp6]"));
}
