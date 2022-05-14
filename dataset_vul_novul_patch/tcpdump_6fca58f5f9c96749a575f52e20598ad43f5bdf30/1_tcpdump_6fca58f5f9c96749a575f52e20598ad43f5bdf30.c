pimv2_print(netdissect_options *ndo,
            register const u_char *bp, register u_int len, const u_char *bp2)
{
	register const u_char *ep;
	register const struct pim *pim = (const struct pim *)bp;
	int advance;
	enum checksum_status cksum_status;

	ep = (const u_char *)ndo->ndo_snapend;
	if (bp >= ep)
		return;
	if (ep > bp + len)
		ep = bp + len;
	ND_TCHECK(pim->pim_rsv);
	pimv2_addr_len = pim->pim_rsv;
	if (pimv2_addr_len != 0)
		ND_PRINT((ndo, ", RFC2117-encoding"));

	ND_PRINT((ndo, ", cksum 0x%04x ", EXTRACT_16BITS(&pim->pim_cksum)));
	if (EXTRACT_16BITS(&pim->pim_cksum) == 0) {
		ND_PRINT((ndo, "(unverified)"));
	} else {
		if (PIM_TYPE(pim->pim_typever) == PIMV2_TYPE_REGISTER) {
			/*
			 * The checksum only covers the packet header,
			 * not the encapsulated packet.
			 */
			cksum_status = pimv2_check_checksum(ndo, bp, bp2, 8);
			if (cksum_status == INCORRECT) {
				/*
				 * To quote RFC 4601, "For interoperability
				 * reasons, a message carrying a checksum
				 * calculated over the entire PIM Register
				 * message should also be accepted."
				 */
				cksum_status = pimv2_check_checksum(ndo, bp, bp2, len);
			}
		} else {
			/*
			 * The checksum covers the entire packet.
			 */
			cksum_status = pimv2_check_checksum(ndo, bp, bp2, len);
		}
		switch (cksum_status) {

		case CORRECT:
			ND_PRINT((ndo, "(correct)"));
			break;

		case INCORRECT:
			ND_PRINT((ndo, "(incorrect)"));
			break;

		case UNVERIFIED:
			ND_PRINT((ndo, "(unverified)"));
			break;
		}
	}

	switch (PIM_TYPE(pim->pim_typever)) {
	case PIMV2_TYPE_HELLO:
	    {
		uint16_t otype, olen;
		bp += 4;
		while (bp < ep) {
			ND_TCHECK2(bp[0], 4);
			otype = EXTRACT_16BITS(&bp[0]);
			olen = EXTRACT_16BITS(&bp[2]);
			ND_TCHECK2(bp[0], 4 + olen);
			ND_PRINT((ndo, "\n\t  %s Option (%u), length %u, Value: ",
			          tok2str(pimv2_hello_option_values, "Unknown", otype),
			          otype,
			          olen));
			bp += 4;
 
 			switch (otype) {
 			case PIMV2_HELLO_OPTION_HOLDTIME:
				unsigned_relts_print(ndo, EXTRACT_16BITS(bp));
 				break;
 
 			case PIMV2_HELLO_OPTION_LANPRUNEDELAY:
				if (olen != 4) {
					ND_PRINT((ndo, "ERROR: Option Length != 4 Bytes (%u)", olen));
				} else {
					char t_bit;
					uint16_t lan_delay, override_interval;
					lan_delay = EXTRACT_16BITS(bp);
					override_interval = EXTRACT_16BITS(bp+2);
					t_bit = (lan_delay & 0x8000)? 1 : 0;
					lan_delay &= ~0x8000;
					ND_PRINT((ndo, "\n\t    T-bit=%d, LAN delay %dms, Override interval %dms",
					t_bit, lan_delay, override_interval));
				}
				break;

			case PIMV2_HELLO_OPTION_DR_PRIORITY_OLD:
			case PIMV2_HELLO_OPTION_DR_PRIORITY:
				switch (olen) {
				case 0:
					ND_PRINT((ndo, "Bi-Directional Capability (Old)"));
					break;
				case 4:
					ND_PRINT((ndo, "%u", EXTRACT_32BITS(bp)));
					break;
				default:
					ND_PRINT((ndo, "ERROR: Option Length != 4 Bytes (%u)", olen));
					break;
				}
 				break;
 
 			case PIMV2_HELLO_OPTION_GENID:
				ND_PRINT((ndo, "0x%08x", EXTRACT_32BITS(bp)));
 				break;
 
 			case PIMV2_HELLO_OPTION_REFRESH_CAP:
				ND_PRINT((ndo, "v%d", *bp));
				if (*(bp+1) != 0) {
					ND_PRINT((ndo, ", interval "));
					unsigned_relts_print(ndo, *(bp+1));
				}
				if (EXTRACT_16BITS(bp+2) != 0) {
					ND_PRINT((ndo, " ?0x%04x?", EXTRACT_16BITS(bp+2)));
 				}
 				break;
 
			case  PIMV2_HELLO_OPTION_BIDIR_CAP:
				break;

			case PIMV2_HELLO_OPTION_ADDRESS_LIST_OLD:
			case PIMV2_HELLO_OPTION_ADDRESS_LIST:
				if (ndo->ndo_vflag > 1) {
					const u_char *ptr = bp;
					while (ptr < (bp+olen)) {
						ND_PRINT((ndo, "\n\t    "));
						advance = pimv2_addr_print(ndo, ptr, pimv2_unicast, 0);
						if (advance < 0) {
							ND_PRINT((ndo, "..."));
							break;
						}
						ptr += advance;
					}
				}
				break;
			default:
				if (ndo->ndo_vflag <= 1)
					print_unknown_data(ndo, bp, "\n\t    ", olen);
				break;
			}
			/* do we want to see an additionally hexdump ? */
			if (ndo->ndo_vflag> 1)
				print_unknown_data(ndo, bp, "\n\t    ", olen);
			bp += olen;
		}
		break;
	    }

	case PIMV2_TYPE_REGISTER:
	{
		const struct ip *ip;

		ND_TCHECK2(*(bp + 4), PIMV2_REGISTER_FLAG_LEN);

		ND_PRINT((ndo, ", Flags [ %s ]\n\t",
		          tok2str(pimv2_register_flag_values,
		          "none",
		          EXTRACT_32BITS(bp+4))));

		bp += 8; len -= 8;
		/* encapsulated multicast packet */
		ip = (const struct ip *)bp;
		switch (IP_V(ip)) {
                case 0: /* Null header */
			ND_PRINT((ndo, "IP-Null-header %s > %s",
			          ipaddr_string(ndo, &ip->ip_src),
			          ipaddr_string(ndo, &ip->ip_dst)));
			break;

		case 4:	/* IPv4 */
			ip_print(ndo, bp, len);
			break;

		case 6:	/* IPv6 */
			ip6_print(ndo, bp, len);
			break;

		default:
			ND_PRINT((ndo, "IP ver %d", IP_V(ip)));
			break;
		}
		break;
	}

	case PIMV2_TYPE_REGISTER_STOP:
		bp += 4; len -= 4;
		if (bp >= ep)
			break;
		ND_PRINT((ndo, " group="));
		if ((advance = pimv2_addr_print(ndo, bp, pimv2_group, 0)) < 0) {
			ND_PRINT((ndo, "..."));
			break;
		}
		bp += advance; len -= advance;
		if (bp >= ep)
			break;
		ND_PRINT((ndo, " source="));
		if ((advance = pimv2_addr_print(ndo, bp, pimv2_unicast, 0)) < 0) {
			ND_PRINT((ndo, "..."));
			break;
		}
		bp += advance; len -= advance;
		break;

	case PIMV2_TYPE_JOIN_PRUNE:
	case PIMV2_TYPE_GRAFT:
	case PIMV2_TYPE_GRAFT_ACK:


        /*
         * 0                   1                   2                   3
         *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |PIM Ver| Type  | Addr length   |           Checksum            |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |             Unicast-Upstream Neighbor Address                 |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |  Reserved     | Num groups    |          Holdtime             |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |            Encoded-Multicast Group Address-1                  |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |   Number of Joined  Sources   |   Number of Pruned Sources    |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |               Encoded-Joined Source Address-1                 |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |                             .                                 |
         *  |                             .                                 |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |               Encoded-Joined Source Address-n                 |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |               Encoded-Pruned Source Address-1                 |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |                             .                                 |
         *  |                             .                                 |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |               Encoded-Pruned Source Address-n                 |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |                           .                                   |
         *  |                           .                                   |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |                Encoded-Multicast Group Address-n              |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

	    {
		uint8_t ngroup;
		uint16_t holdtime;
		uint16_t njoin;
		uint16_t nprune;
		int i, j;

		bp += 4; len -= 4;
		if (PIM_TYPE(pim->pim_typever) != 7) {	/*not for Graft-ACK*/
			if (bp >= ep)
				break;
			ND_PRINT((ndo, ", upstream-neighbor: "));
			if ((advance = pimv2_addr_print(ndo, bp, pimv2_unicast, 0)) < 0) {
				ND_PRINT((ndo, "..."));
				break;
			}
			bp += advance; len -= advance;
		}
		if (bp + 4 > ep)
			break;
		ngroup = bp[1];
		holdtime = EXTRACT_16BITS(&bp[2]);
		ND_PRINT((ndo, "\n\t  %u group(s)", ngroup));
		if (PIM_TYPE(pim->pim_typever) != 7) {	/*not for Graft-ACK*/
			ND_PRINT((ndo, ", holdtime: "));
			if (holdtime == 0xffff)
				ND_PRINT((ndo, "infinite"));
			else
				unsigned_relts_print(ndo, holdtime);
		}
		bp += 4; len -= 4;
		for (i = 0; i < ngroup; i++) {
			if (bp >= ep)
				goto jp_done;
			ND_PRINT((ndo, "\n\t    group #%u: ", i+1));
			if ((advance = pimv2_addr_print(ndo, bp, pimv2_group, 0)) < 0) {
				ND_PRINT((ndo, "...)"));
				goto jp_done;
			}
			bp += advance; len -= advance;
			if (bp + 4 > ep) {
				ND_PRINT((ndo, "...)"));
				goto jp_done;
			}
			njoin = EXTRACT_16BITS(&bp[0]);
			nprune = EXTRACT_16BITS(&bp[2]);
			ND_PRINT((ndo, ", joined sources: %u, pruned sources: %u", njoin, nprune));
			bp += 4; len -= 4;
			for (j = 0; j < njoin; j++) {
				ND_PRINT((ndo, "\n\t      joined source #%u: ", j+1));
				if ((advance = pimv2_addr_print(ndo, bp, pimv2_source, 0)) < 0) {
					ND_PRINT((ndo, "...)"));
					goto jp_done;
				}
				bp += advance; len -= advance;
			}
			for (j = 0; j < nprune; j++) {
				ND_PRINT((ndo, "\n\t      pruned source #%u: ", j+1));
				if ((advance = pimv2_addr_print(ndo, bp, pimv2_source, 0)) < 0) {
					ND_PRINT((ndo, "...)"));
					goto jp_done;
				}
				bp += advance; len -= advance;
			}
		}
	jp_done:
		break;
	    }

	case PIMV2_TYPE_BOOTSTRAP:
	{
		int i, j, frpcnt;
		bp += 4;

		/* Fragment Tag, Hash Mask len, and BSR-priority */
		if (bp + sizeof(uint16_t) >= ep) break;
		ND_PRINT((ndo, " tag=%x", EXTRACT_16BITS(bp)));
		bp += sizeof(uint16_t);
		if (bp >= ep) break;
		ND_PRINT((ndo, " hashmlen=%d", bp[0]));
		if (bp + 1 >= ep) break;
		ND_PRINT((ndo, " BSRprio=%d", bp[1]));
		bp += 2;

		/* Encoded-Unicast-BSR-Address */
		if (bp >= ep) break;
		ND_PRINT((ndo, " BSR="));
		if ((advance = pimv2_addr_print(ndo, bp, pimv2_unicast, 0)) < 0) {
			ND_PRINT((ndo, "..."));
			break;
		}
		bp += advance;

		for (i = 0; bp < ep; i++) {
			/* Encoded-Group Address */
			ND_PRINT((ndo, " (group%d: ", i));
			if ((advance = pimv2_addr_print(ndo, bp, pimv2_group, 0))
			    < 0) {
				ND_PRINT((ndo, "...)"));
				goto bs_done;
			}
			bp += advance;

			/* RP-Count, Frag RP-Cnt, and rsvd */
			if (bp >= ep) {
				ND_PRINT((ndo, "...)"));
				goto bs_done;
			}
			ND_PRINT((ndo, " RPcnt=%d", bp[0]));
			if (bp + 1 >= ep) {
				ND_PRINT((ndo, "...)"));
				goto bs_done;
			}
			ND_PRINT((ndo, " FRPcnt=%d", frpcnt = bp[1]));
			bp += 4;

			for (j = 0; j < frpcnt && bp < ep; j++) {
				/* each RP info */
				ND_PRINT((ndo, " RP%d=", j));
				if ((advance = pimv2_addr_print(ndo, bp,
								pimv2_unicast,
								0)) < 0) {
					ND_PRINT((ndo, "...)"));
					goto bs_done;
				}
				bp += advance;

				if (bp + 1 >= ep) {
					ND_PRINT((ndo, "...)"));
					goto bs_done;
				}
				ND_PRINT((ndo, ",holdtime="));
				unsigned_relts_print(ndo, EXTRACT_16BITS(bp));
				if (bp + 2 >= ep) {
					ND_PRINT((ndo, "...)"));
					goto bs_done;
				}
				ND_PRINT((ndo, ",prio=%d", bp[2]));
				bp += 4;
			}
			ND_PRINT((ndo, ")"));
		}
	   bs_done:
		break;
	}
	case PIMV2_TYPE_ASSERT:
		bp += 4; len -= 4;
		if (bp >= ep)
			break;
		ND_PRINT((ndo, " group="));
		if ((advance = pimv2_addr_print(ndo, bp, pimv2_group, 0)) < 0) {
			ND_PRINT((ndo, "..."));
			break;
		}
		bp += advance; len -= advance;
		if (bp >= ep)
			break;
		ND_PRINT((ndo, " src="));
		if ((advance = pimv2_addr_print(ndo, bp, pimv2_unicast, 0)) < 0) {
			ND_PRINT((ndo, "..."));
			break;
		}
		bp += advance; len -= advance;
		if (bp + 8 > ep)
			break;
		if (bp[0] & 0x80)
			ND_PRINT((ndo, " RPT"));
		ND_PRINT((ndo, " pref=%u", EXTRACT_32BITS(&bp[0]) & 0x7fffffff));
		ND_PRINT((ndo, " metric=%u", EXTRACT_32BITS(&bp[4])));
		break;

	case PIMV2_TYPE_CANDIDATE_RP:
	{
		int i, pfxcnt;
		bp += 4;

		/* Prefix-Cnt, Priority, and Holdtime */
		if (bp >= ep) break;
		ND_PRINT((ndo, " prefix-cnt=%d", bp[0]));
		pfxcnt = bp[0];
		if (bp + 1 >= ep) break;
		ND_PRINT((ndo, " prio=%d", bp[1]));
		if (bp + 3 >= ep) break;
		ND_PRINT((ndo, " holdtime="));
		unsigned_relts_print(ndo, EXTRACT_16BITS(&bp[2]));
		bp += 4;

		/* Encoded-Unicast-RP-Address */
		if (bp >= ep) break;
		ND_PRINT((ndo, " RP="));
		if ((advance = pimv2_addr_print(ndo, bp, pimv2_unicast, 0)) < 0) {
			ND_PRINT((ndo, "..."));
			break;
		}
		bp += advance;

		/* Encoded-Group Addresses */
		for (i = 0; i < pfxcnt && bp < ep; i++) {
			ND_PRINT((ndo, " Group%d=", i));
			if ((advance = pimv2_addr_print(ndo, bp, pimv2_group, 0))
			    < 0) {
				ND_PRINT((ndo, "..."));
				break;
			}
			bp += advance;
		}
		break;
	}

	case PIMV2_TYPE_PRUNE_REFRESH:
		ND_PRINT((ndo, " src="));
		if ((advance = pimv2_addr_print(ndo, bp, pimv2_unicast, 0)) < 0) {
			ND_PRINT((ndo, "..."));
			break;
		}
		bp += advance;
		ND_PRINT((ndo, " grp="));
		if ((advance = pimv2_addr_print(ndo, bp, pimv2_group, 0)) < 0) {
			ND_PRINT((ndo, "..."));
			break;
		}
		bp += advance;
		ND_PRINT((ndo, " forwarder="));
		if ((advance = pimv2_addr_print(ndo, bp, pimv2_unicast, 0)) < 0) {
			ND_PRINT((ndo, "..."));
			break;
		}
		bp += advance;
		ND_TCHECK2(bp[0], 2);
		ND_PRINT((ndo, " TUNR "));
		unsigned_relts_print(ndo, EXTRACT_16BITS(bp));
		break;


	 default:
		ND_PRINT((ndo, " [type %d]", PIM_TYPE(pim->pim_typever)));
		break;
	}

	return;

trunc:
	ND_PRINT((ndo, "[|pim]"));
}
