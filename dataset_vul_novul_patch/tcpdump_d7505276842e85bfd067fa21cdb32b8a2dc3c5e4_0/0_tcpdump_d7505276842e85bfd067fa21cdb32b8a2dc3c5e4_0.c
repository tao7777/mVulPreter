icmp6_opt_print(netdissect_options *ndo, const u_char *bp, int resid)
{
	const struct nd_opt_hdr *op;
	const struct nd_opt_prefix_info *opp;
	const struct nd_opt_mtu *opm;
	const struct nd_opt_rdnss *oprd;
	const struct nd_opt_dnssl *opds;
	const struct nd_opt_advinterval *opa;
	const struct nd_opt_homeagent_info *oph;
	const struct nd_opt_route_info *opri;
	const u_char *cp, *ep, *domp;
	struct in6_addr in6;
	const struct in6_addr *in6p;
	size_t l;
	u_int i;

#define ECHECK(var) if ((const u_char *)&(var) > ep - sizeof(var)) return

	cp = bp;
	/* 'ep' points to the end of available data. */
	ep = ndo->ndo_snapend;

	while (cp < ep) {
		op = (const struct nd_opt_hdr *)cp;

		ECHECK(op->nd_opt_len);
		if (resid <= 0)
			return;
		if (op->nd_opt_len == 0)
			goto trunc;
		if (cp + (op->nd_opt_len << 3) > ep)
			goto trunc;

                ND_PRINT((ndo,"\n\t  %s option (%u), length %u (%u): ",
                          tok2str(icmp6_opt_values, "unknown", op->nd_opt_type),
                          op->nd_opt_type,
                          op->nd_opt_len << 3,
                          op->nd_opt_len));

		switch (op->nd_opt_type) {
		case ND_OPT_SOURCE_LINKADDR:
			l = (op->nd_opt_len << 3) - 2;
			print_lladdr(ndo, cp + 2, l);
			break;
		case ND_OPT_TARGET_LINKADDR:
			l = (op->nd_opt_len << 3) - 2;
			print_lladdr(ndo, cp + 2, l);
			break;
		case ND_OPT_PREFIX_INFORMATION:
			opp = (const struct nd_opt_prefix_info *)op;
			ND_TCHECK(opp->nd_opt_pi_prefix);
                        ND_PRINT((ndo,"%s/%u%s, Flags [%s], valid time %s",
                                  ip6addr_string(ndo, &opp->nd_opt_pi_prefix),
                                  opp->nd_opt_pi_prefix_len,
                                  (op->nd_opt_len != 4) ? "badlen" : "",
                                  bittok2str(icmp6_opt_pi_flag_values, "none", opp->nd_opt_pi_flags_reserved),
                                  get_lifetime(EXTRACT_32BITS(&opp->nd_opt_pi_valid_time))));
                        ND_PRINT((ndo,", pref. time %s", get_lifetime(EXTRACT_32BITS(&opp->nd_opt_pi_preferred_time))));
			break;
		case ND_OPT_REDIRECTED_HEADER:
                        print_unknown_data(ndo, bp,"\n\t    ",op->nd_opt_len<<3);
			/* xxx */
			break;
		case ND_OPT_MTU:
			opm = (const struct nd_opt_mtu *)op;
			ND_TCHECK(opm->nd_opt_mtu_mtu);
			ND_PRINT((ndo," %u%s",
                               EXTRACT_32BITS(&opm->nd_opt_mtu_mtu),
                                  (op->nd_opt_len != 1) ? "bad option length" : "" ));
                        break;
		case ND_OPT_RDNSS:
			oprd = (const struct nd_opt_rdnss *)op;
			l = (op->nd_opt_len - 1) / 2;
			ND_PRINT((ndo," lifetime %us,",
                                  EXTRACT_32BITS(&oprd->nd_opt_rdnss_lifetime)));
			for (i = 0; i < l; i++) {
				ND_TCHECK(oprd->nd_opt_rdnss_addr[i]);
				ND_PRINT((ndo," addr: %s",
                                          ip6addr_string(ndo, &oprd->nd_opt_rdnss_addr[i])));
			}
			break;
		case ND_OPT_DNSSL:
			opds = (const struct nd_opt_dnssl *)op;
			ND_PRINT((ndo," lifetime %us, domain(s):",
                                  EXTRACT_32BITS(&opds->nd_opt_dnssl_lifetime)));
			domp = cp + 8; /* domain names, variable-sized, RFC1035-encoded */
			while (domp < cp + (op->nd_opt_len << 3) && *domp != '\0')
			{
				ND_PRINT((ndo, " "));
				if ((domp = ns_nprint (ndo, domp, bp)) == NULL)
					goto trunc;
			}
			break;
		case ND_OPT_ADVINTERVAL:
			opa = (const struct nd_opt_advinterval *)op;
			ND_TCHECK(opa->nd_opt_adv_interval);
			ND_PRINT((ndo," %ums", EXTRACT_32BITS(&opa->nd_opt_adv_interval)));
			break;
                case ND_OPT_HOMEAGENT_INFO:
			oph = (const struct nd_opt_homeagent_info *)op;
			ND_TCHECK(oph->nd_opt_hai_lifetime);
			ND_PRINT((ndo," preference %u, lifetime %u",
                                  EXTRACT_16BITS(&oph->nd_opt_hai_preference),
                                  EXTRACT_16BITS(&oph->nd_opt_hai_lifetime)));
			break;
		case ND_OPT_ROUTE_INFO:
			opri = (const struct nd_opt_route_info *)op;
			ND_TCHECK(opri->nd_opt_rti_lifetime);
			memset(&in6, 0, sizeof(in6));
			in6p = (const struct in6_addr *)(opri + 1);
			switch (op->nd_opt_len) {
			case 1:
				break;
			case 2:
				ND_TCHECK2(*in6p, 8);
				memcpy(&in6, opri + 1, 8);
				break;
			case 3:
				ND_TCHECK(*in6p);
				memcpy(&in6, opri + 1, sizeof(in6));
				break;
			default:
				goto trunc;
			}
			ND_PRINT((ndo," %s/%u", ip6addr_string(ndo, &in6),
                                  opri->nd_opt_rti_prefixlen));
			ND_PRINT((ndo,", pref=%s", get_rtpref(opri->nd_opt_rti_flags)));
			ND_PRINT((ndo,", lifetime=%s",
                                  get_lifetime(EXTRACT_32BITS(&opri->nd_opt_rti_lifetime))));
			break;
		default:
                        if (ndo->ndo_vflag <= 1) {
                                print_unknown_data(ndo,cp+2,"\n\t  ", (op->nd_opt_len << 3) - 2); /* skip option header */
                            return;
                        }
                        break;
		}
                /* do we want to see an additional hexdump ? */
                if (ndo->ndo_vflag> 1)
                        print_unknown_data(ndo, cp+2,"\n\t    ", (op->nd_opt_len << 3) - 2); /* skip option header */

		cp += op->nd_opt_len << 3;
		resid -= op->nd_opt_len << 3;
 	}
 	return;
 
trunc:
	ND_PRINT((ndo, "%s", icmp6_tstr));
 	return;
 #undef ECHECK
 }
