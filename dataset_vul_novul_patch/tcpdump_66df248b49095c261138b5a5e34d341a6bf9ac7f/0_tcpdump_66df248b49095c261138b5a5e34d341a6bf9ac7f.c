ip6_print(netdissect_options *ndo, const u_char *bp, u_int length)
{
	register const struct ip6_hdr *ip6;
	register int advance;
	u_int len;
	const u_char *ipend;
	register const u_char *cp;
	register u_int payload_len;
	int nh;
	int fragmented = 0;
	u_int flow;

	ip6 = (const struct ip6_hdr *)bp;

	ND_TCHECK(*ip6);
	if (length < sizeof (struct ip6_hdr)) {
		ND_PRINT((ndo, "truncated-ip6 %u", length));
		return;
	}

        if (!ndo->ndo_eflag)
            ND_PRINT((ndo, "IP6 "));

	if (IP6_VERSION(ip6) != 6) {
          ND_PRINT((ndo,"version error: %u != 6", IP6_VERSION(ip6)));
          return;
	}

	payload_len = EXTRACT_16BITS(&ip6->ip6_plen);
	len = payload_len + sizeof(struct ip6_hdr);
	if (length < len)
		ND_PRINT((ndo, "truncated-ip6 - %u bytes missing!",
			len - length));

        if (ndo->ndo_vflag) {
            flow = EXTRACT_32BITS(&ip6->ip6_flow);
            ND_PRINT((ndo, "("));
#if 0
            /* rfc1883 */
            if (flow & 0x0f000000)
		ND_PRINT((ndo, "pri 0x%02x, ", (flow & 0x0f000000) >> 24));
            if (flow & 0x00ffffff)
		ND_PRINT((ndo, "flowlabel 0x%06x, ", flow & 0x00ffffff));
#else
            /* RFC 2460 */
            if (flow & 0x0ff00000)
		ND_PRINT((ndo, "class 0x%02x, ", (flow & 0x0ff00000) >> 20));
            if (flow & 0x000fffff)
		ND_PRINT((ndo, "flowlabel 0x%05x, ", flow & 0x000fffff));
#endif

            ND_PRINT((ndo, "hlim %u, next-header %s (%u) payload length: %u) ",
                         ip6->ip6_hlim,
                         tok2str(ipproto_values,"unknown",ip6->ip6_nxt),
                         ip6->ip6_nxt,
                         payload_len));
        }

	/*
	 * Cut off the snapshot length to the end of the IP payload.
	 */
	ipend = bp + len;
	if (ipend < ndo->ndo_snapend)
		ndo->ndo_snapend = ipend;

	cp = (const u_char *)ip6;
 	advance = sizeof(struct ip6_hdr);
 	nh = ip6->ip6_nxt;
 	while (cp < ndo->ndo_snapend && advance > 0) {
		if (len < (u_int)advance)
			goto trunc;
 		cp += advance;
 		len -= advance;
 
		if (cp == (const u_char *)(ip6 + 1) &&
		    nh != IPPROTO_TCP && nh != IPPROTO_UDP &&
		    nh != IPPROTO_DCCP && nh != IPPROTO_SCTP) {
			ND_PRINT((ndo, "%s > %s: ", ip6addr_string(ndo, &ip6->ip6_src),
				     ip6addr_string(ndo, &ip6->ip6_dst)));
		}

		switch (nh) {
		case IPPROTO_HOPOPTS:
			advance = hbhopt_print(ndo, cp);
			if (advance < 0)
				return;
			nh = *cp;
			break;
		case IPPROTO_DSTOPTS:
			advance = dstopt_print(ndo, cp);
			if (advance < 0)
				return;
			nh = *cp;
			break;
		case IPPROTO_FRAGMENT:
			advance = frag6_print(ndo, cp, (const u_char *)ip6);
			if (advance < 0 || ndo->ndo_snapend <= cp + advance)
				return;
			nh = *cp;
			fragmented = 1;
			break;

		case IPPROTO_MOBILITY_OLD:
		case IPPROTO_MOBILITY:
			/*
			 * XXX - we don't use "advance"; RFC 3775 says that
			 * the next header field in a mobility header
			 * should be IPPROTO_NONE, but speaks of
			 * the possiblity of a future extension in
			 * which payload can be piggybacked atop a
 			 * mobility header.
 			 */
 			advance = mobility_print(ndo, cp, (const u_char *)ip6);
			if (advance < 0)
				return;
 			nh = *cp;
 			return;
 		case IPPROTO_ROUTING:
			ND_TCHECK(*cp);
 			advance = rt6_print(ndo, cp, (const u_char *)ip6);
			if (advance < 0)
				return;
 			nh = *cp;
 			break;
 		case IPPROTO_SCTP:
			sctp_print(ndo, cp, (const u_char *)ip6, len);
			return;
		case IPPROTO_DCCP:
			dccp_print(ndo, cp, (const u_char *)ip6, len);
			return;
		case IPPROTO_TCP:
			tcp_print(ndo, cp, len, (const u_char *)ip6, fragmented);
			return;
		case IPPROTO_UDP:
			udp_print(ndo, cp, len, (const u_char *)ip6, fragmented);
			return;
		case IPPROTO_ICMPV6:
			icmp6_print(ndo, cp, len, (const u_char *)ip6, fragmented);
 			return;
 		case IPPROTO_AH:
 			advance = ah_print(ndo, cp);
			if (advance < 0)
				return;
 			nh = *cp;
 			break;
 		case IPPROTO_ESP:
 		    {
 			int enh, padlen;
 			advance = esp_print(ndo, cp, len, (const u_char *)ip6, &enh, &padlen);
			if (advance < 0)
				return;
 			nh = enh & 0xff;
 			len -= padlen;
 			break;
		    }
		case IPPROTO_IPCOMP:
		    {
			ipcomp_print(ndo, cp);
			/*
			 * Either this has decompressed the payload and
			 * printed it, in which case there's nothing more
			 * to do, or it hasn't, in which case there's
			 * nothing more to do.
			 */
			advance = -1;
			break;
		    }

		case IPPROTO_PIM:
			pim_print(ndo, cp, len, (const u_char *)ip6);
			return;

		case IPPROTO_OSPF:
			ospf6_print(ndo, cp, len);
			return;

		case IPPROTO_IPV6:
			ip6_print(ndo, cp, len);
			return;

		case IPPROTO_IPV4:
		        ip_print(ndo, cp, len);
			return;

                case IPPROTO_PGM:
                        pgm_print(ndo, cp, len, (const u_char *)ip6);
                        return;

		case IPPROTO_GRE:
			gre_print(ndo, cp, len);
			return;

		case IPPROTO_RSVP:
			rsvp_print(ndo, cp, len);
			return;

		case IPPROTO_NONE:
			ND_PRINT((ndo, "no next header"));
			return;

		default:
			ND_PRINT((ndo, "ip-proto-%d %d", nh, len));
			return;
		}
	}

	return;
trunc:
	ND_PRINT((ndo, "[|ip6]"));
}
