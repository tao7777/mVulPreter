icmp_print(netdissect_options *ndo, const u_char *bp, u_int plen, const u_char *bp2,
           int fragmented)
{
	char *cp;
	const struct icmp *dp;
        const struct icmp_ext_t *ext_dp;
	const struct ip *ip;
	const char *str, *fmt;
	const struct ip *oip;
	const struct udphdr *ouh;
        const uint8_t *obj_tptr;
        uint32_t raw_label;
        const u_char *snapend_save;
	const struct icmp_mpls_ext_object_header_t *icmp_mpls_ext_object_header;
	u_int hlen, dport, mtu, obj_tlen, obj_class_num, obj_ctype;
	char buf[MAXHOSTNAMELEN + 100];
	struct cksum_vec vec[1];

	dp = (const struct icmp *)bp;
        ext_dp = (const struct icmp_ext_t *)bp;
	ip = (const struct ip *)bp2;
	str = buf;

	ND_TCHECK(dp->icmp_code);
	switch (dp->icmp_type) {

	case ICMP_ECHO:
	case ICMP_ECHOREPLY:
		ND_TCHECK(dp->icmp_seq);
		(void)snprintf(buf, sizeof(buf), "echo %s, id %u, seq %u",
                               dp->icmp_type == ICMP_ECHO ?
                               "request" : "reply",
                               EXTRACT_16BITS(&dp->icmp_id),
                               EXTRACT_16BITS(&dp->icmp_seq));
		break;

	case ICMP_UNREACH:
		ND_TCHECK(dp->icmp_ip.ip_dst);
		switch (dp->icmp_code) {

		case ICMP_UNREACH_PROTOCOL:
			ND_TCHECK(dp->icmp_ip.ip_p);
			(void)snprintf(buf, sizeof(buf),
			    "%s protocol %d unreachable",
			    ipaddr_string(ndo, &dp->icmp_ip.ip_dst),
			    dp->icmp_ip.ip_p);
			break;

		case ICMP_UNREACH_PORT:
			ND_TCHECK(dp->icmp_ip.ip_p);
			oip = &dp->icmp_ip;
			hlen = IP_HL(oip) * 4;
			ouh = (const struct udphdr *)(((const u_char *)oip) + hlen);
			ND_TCHECK(ouh->uh_dport);
			dport = EXTRACT_16BITS(&ouh->uh_dport);
			switch (oip->ip_p) {

			case IPPROTO_TCP:
				(void)snprintf(buf, sizeof(buf),
					"%s tcp port %s unreachable",
					ipaddr_string(ndo, &oip->ip_dst),
					tcpport_string(ndo, dport));
				break;

			case IPPROTO_UDP:
				(void)snprintf(buf, sizeof(buf),
					"%s udp port %s unreachable",
					ipaddr_string(ndo, &oip->ip_dst),
					udpport_string(ndo, dport));
				break;

			default:
				(void)snprintf(buf, sizeof(buf),
					"%s protocol %d port %d unreachable",
					ipaddr_string(ndo, &oip->ip_dst),
					oip->ip_p, dport);
				break;
			}
			break;

		case ICMP_UNREACH_NEEDFRAG:
		    {
			register const struct mtu_discovery *mp;
			mp = (const struct mtu_discovery *)(const u_char *)&dp->icmp_void;
			mtu = EXTRACT_16BITS(&mp->nexthopmtu);
			if (mtu) {
				(void)snprintf(buf, sizeof(buf),
				    "%s unreachable - need to frag (mtu %d)",
				    ipaddr_string(ndo, &dp->icmp_ip.ip_dst), mtu);
			} else {
				(void)snprintf(buf, sizeof(buf),
				    "%s unreachable - need to frag",
				    ipaddr_string(ndo, &dp->icmp_ip.ip_dst));
			}
		    }
			break;

		default:
			fmt = tok2str(unreach2str, "#%d %%s unreachable",
			    dp->icmp_code);
			(void)snprintf(buf, sizeof(buf), fmt,
			    ipaddr_string(ndo, &dp->icmp_ip.ip_dst));
			break;
		}
		break;

	case ICMP_REDIRECT:
		ND_TCHECK(dp->icmp_ip.ip_dst);
		fmt = tok2str(type2str, "redirect-#%d %%s to net %%s",
		    dp->icmp_code);
		(void)snprintf(buf, sizeof(buf), fmt,
		    ipaddr_string(ndo, &dp->icmp_ip.ip_dst),
		    ipaddr_string(ndo, &dp->icmp_gwaddr));
		break;

	case ICMP_ROUTERADVERT:
	    {
		register const struct ih_rdiscovery *ihp;
		register const struct id_rdiscovery *idp;
		u_int lifetime, num, size;

		(void)snprintf(buf, sizeof(buf), "router advertisement");
		cp = buf + strlen(buf);

		ihp = (const struct ih_rdiscovery *)&dp->icmp_void;
		ND_TCHECK(*ihp);
		(void)strncpy(cp, " lifetime ", sizeof(buf) - (cp - buf));
		cp = buf + strlen(buf);
		lifetime = EXTRACT_16BITS(&ihp->ird_lifetime);
		if (lifetime < 60) {
			(void)snprintf(cp, sizeof(buf) - (cp - buf), "%u",
			    lifetime);
		} else if (lifetime < 60 * 60) {
			(void)snprintf(cp, sizeof(buf) - (cp - buf), "%u:%02u",
			    lifetime / 60, lifetime % 60);
		} else {
			(void)snprintf(cp, sizeof(buf) - (cp - buf),
			    "%u:%02u:%02u",
			    lifetime / 3600,
			    (lifetime % 3600) / 60,
			    lifetime % 60);
		}
		cp = buf + strlen(buf);

		num = ihp->ird_addrnum;
		(void)snprintf(cp, sizeof(buf) - (cp - buf), " %d:", num);
		cp = buf + strlen(buf);

		size = ihp->ird_addrsiz;
		if (size != 2) {
			(void)snprintf(cp, sizeof(buf) - (cp - buf),
			    " [size %d]", size);
			break;
		}
		idp = (const struct id_rdiscovery *)&dp->icmp_data;
		while (num-- > 0) {
			ND_TCHECK(*idp);
			(void)snprintf(cp, sizeof(buf) - (cp - buf), " {%s %u}",
			    ipaddr_string(ndo, &idp->ird_addr),
			    EXTRACT_32BITS(&idp->ird_pref));
			cp = buf + strlen(buf);
			++idp;
		}
	    }
		break;

	case ICMP_TIMXCEED:
		ND_TCHECK(dp->icmp_ip.ip_dst);
		switch (dp->icmp_code) {

		case ICMP_TIMXCEED_INTRANS:
			str = "time exceeded in-transit";
			break;

		case ICMP_TIMXCEED_REASS:
			str = "ip reassembly time exceeded";
			break;

		default:
			(void)snprintf(buf, sizeof(buf), "time exceeded-#%d",
			    dp->icmp_code);
			break;
		}
		break;

	case ICMP_PARAMPROB:
		if (dp->icmp_code)
			(void)snprintf(buf, sizeof(buf),
			    "parameter problem - code %d", dp->icmp_code);
		else {
			ND_TCHECK(dp->icmp_pptr);
			(void)snprintf(buf, sizeof(buf),
			    "parameter problem - octet %d", dp->icmp_pptr);
		}
		break;

	case ICMP_MASKREPLY:
		ND_TCHECK(dp->icmp_mask);
		(void)snprintf(buf, sizeof(buf), "address mask is 0x%08x",
		    EXTRACT_32BITS(&dp->icmp_mask));
		break;

	case ICMP_TSTAMP:
		ND_TCHECK(dp->icmp_seq);
		(void)snprintf(buf, sizeof(buf),
		    "time stamp query id %u seq %u",
		    EXTRACT_16BITS(&dp->icmp_id),
		    EXTRACT_16BITS(&dp->icmp_seq));
		break;

	case ICMP_TSTAMPREPLY:
		ND_TCHECK(dp->icmp_ttime);
		(void)snprintf(buf, sizeof(buf),
		    "time stamp reply id %u seq %u: org %s",
                               EXTRACT_16BITS(&dp->icmp_id),
                               EXTRACT_16BITS(&dp->icmp_seq),
                               icmp_tstamp_print(EXTRACT_32BITS(&dp->icmp_otime)));

                (void)snprintf(buf+strlen(buf),sizeof(buf)-strlen(buf),", recv %s",
                         icmp_tstamp_print(EXTRACT_32BITS(&dp->icmp_rtime)));
                (void)snprintf(buf+strlen(buf),sizeof(buf)-strlen(buf),", xmit %s",
                         icmp_tstamp_print(EXTRACT_32BITS(&dp->icmp_ttime)));
                break;

	default:
		str = tok2str(icmp2str, "type-#%d", dp->icmp_type);
		break;
	}
	ND_PRINT((ndo, "ICMP %s, length %u", str, plen));
	if (ndo->ndo_vflag && !fragmented) { /* don't attempt checksumming if this is a frag */
		uint16_t sum, icmp_sum;

		if (ND_TTEST2(*bp, plen)) {
			vec[0].ptr = (const uint8_t *)(const void *)dp;
			vec[0].len = plen;
			sum = in_cksum(vec, 1);
			if (sum != 0) {
				icmp_sum = EXTRACT_16BITS(&dp->icmp_cksum);
				ND_PRINT((ndo, " (wrong icmp cksum %x (->%x)!)",
					     icmp_sum,
					     in_cksum_shouldbe(icmp_sum, sum)));
			}
		}
	}

        /*
         * print the remnants of the IP packet.
         * save the snaplength as this may get overidden in the IP printer.
         */
	if (ndo->ndo_vflag >= 1 && ICMP_ERRTYPE(dp->icmp_type)) {
		bp += 8;
 		ND_PRINT((ndo, "\n\t"));
 		ip = (const struct ip *)bp;
                 snapend_save = ndo->ndo_snapend;
 		ip_print(ndo, bp, EXTRACT_16BITS(&ip->ip_len));
                 ndo->ndo_snapend = snapend_save;
 	}

        /*
         * Attempt to decode the MPLS extensions only for some ICMP types.
         */
        if (ndo->ndo_vflag >= 1 && plen > ICMP_EXTD_MINLEN && ICMP_MPLS_EXT_TYPE(dp->icmp_type)) {

            ND_TCHECK(*ext_dp);

            /*
             * Check first if the mpls extension header shows a non-zero length.
             * If the length field is not set then silently verify the checksum
             * to check if an extension header is present. This is expedient,
             * however not all implementations set the length field proper.
             */
            if (!ext_dp->icmp_length &&
                ND_TTEST2(ext_dp->icmp_ext_version_res, plen - ICMP_EXTD_MINLEN)) {
                vec[0].ptr = (const uint8_t *)(const void *)&ext_dp->icmp_ext_version_res;
                vec[0].len = plen - ICMP_EXTD_MINLEN;
                if (in_cksum(vec, 1)) {
                    return;
                }
            }

            ND_PRINT((ndo, "\n\tMPLS extension v%u",
                   ICMP_MPLS_EXT_EXTRACT_VERSION(*(ext_dp->icmp_ext_version_res))));

            /*
             * Sanity checking of the header.
             */
            if (ICMP_MPLS_EXT_EXTRACT_VERSION(*(ext_dp->icmp_ext_version_res)) !=
                ICMP_MPLS_EXT_VERSION) {
                ND_PRINT((ndo, " packet not supported"));
                return;
            }

            hlen = plen - ICMP_EXTD_MINLEN;
            if (ND_TTEST2(ext_dp->icmp_ext_version_res, hlen)) {
                vec[0].ptr = (const uint8_t *)(const void *)&ext_dp->icmp_ext_version_res;
                vec[0].len = hlen;
                ND_PRINT((ndo, ", checksum 0x%04x (%scorrect), length %u",
                       EXTRACT_16BITS(ext_dp->icmp_ext_checksum),
                       in_cksum(vec, 1) ? "in" : "",
                       hlen));
            }

            hlen -= 4; /* subtract common header size */
            obj_tptr = (const uint8_t *)ext_dp->icmp_ext_data;

            while (hlen > sizeof(struct icmp_mpls_ext_object_header_t)) {

                icmp_mpls_ext_object_header = (const struct icmp_mpls_ext_object_header_t *)obj_tptr;
                ND_TCHECK(*icmp_mpls_ext_object_header);
                obj_tlen = EXTRACT_16BITS(icmp_mpls_ext_object_header->length);
                obj_class_num = icmp_mpls_ext_object_header->class_num;
                obj_ctype = icmp_mpls_ext_object_header->ctype;
                obj_tptr += sizeof(struct icmp_mpls_ext_object_header_t);

                ND_PRINT((ndo, "\n\t  %s Object (%u), Class-Type: %u, length %u",
                       tok2str(icmp_mpls_ext_obj_values,"unknown",obj_class_num),
                       obj_class_num,
                       obj_ctype,
                       obj_tlen));

                hlen-=sizeof(struct icmp_mpls_ext_object_header_t); /* length field includes tlv header */

                /* infinite loop protection */
                if ((obj_class_num == 0) ||
                    (obj_tlen < sizeof(struct icmp_mpls_ext_object_header_t))) {
                    return;
                }
                obj_tlen-=sizeof(struct icmp_mpls_ext_object_header_t);

                switch (obj_class_num) {
                case 1:
                    switch(obj_ctype) {
                    case 1:
                        ND_TCHECK2(*obj_tptr, 4);
                        raw_label = EXTRACT_32BITS(obj_tptr);
                        ND_PRINT((ndo, "\n\t    label %u, exp %u", MPLS_LABEL(raw_label), MPLS_EXP(raw_label)));
                        if (MPLS_STACK(raw_label))
                            ND_PRINT((ndo, ", [S]"));
                        ND_PRINT((ndo, ", ttl %u", MPLS_TTL(raw_label)));
                        break;
                    default:
                        print_unknown_data(ndo, obj_tptr, "\n\t    ", obj_tlen);
                    }
                    break;

               /*
                *  FIXME those are the defined objects that lack a decoder
                *  you are welcome to contribute code ;-)
                */
                case 2:
                default:
                    print_unknown_data(ndo, obj_tptr, "\n\t    ", obj_tlen);
                    break;
                }
                if (hlen < obj_tlen)
                    break;
                hlen -= obj_tlen;
                obj_tptr += obj_tlen;
            }
        }

	return;
trunc:
	ND_PRINT((ndo, "[|icmp]"));
}
