icmp6_rrenum_print(netdissect_options *ndo, const u_char *bp, const u_char *ep)
{
	const struct icmp6_router_renum *rr6;
	const char *cp;
	const struct rr_pco_match *match;
	const struct rr_pco_use *use;
	char hbuf[NI_MAXHOST];
	int n;

	if (ep < bp)
		return;
	rr6 = (const struct icmp6_router_renum *)bp;
	cp = (const char *)(rr6 + 1);

	ND_TCHECK(rr6->rr_reserved);
	switch (rr6->rr_code) {
	case ICMP6_ROUTER_RENUMBERING_COMMAND:
		ND_PRINT((ndo,"router renum: command"));
		break;
	case ICMP6_ROUTER_RENUMBERING_RESULT:
		ND_PRINT((ndo,"router renum: result"));
		break;
	case ICMP6_ROUTER_RENUMBERING_SEQNUM_RESET:
		ND_PRINT((ndo,"router renum: sequence number reset"));
		break;
	default:
		ND_PRINT((ndo,"router renum: code-#%d", rr6->rr_code));
		break;
	}

        ND_PRINT((ndo,", seq=%u", EXTRACT_32BITS(&rr6->rr_seqnum)));

	if (ndo->ndo_vflag) {
#define F(x, y)	((rr6->rr_flags) & (x) ? (y) : "")
		ND_PRINT((ndo,"["));	/*]*/
		if (rr6->rr_flags) {
			ND_PRINT((ndo,"%s%s%s%s%s,", F(ICMP6_RR_FLAGS_TEST, "T"),
                                  F(ICMP6_RR_FLAGS_REQRESULT, "R"),
                                  F(ICMP6_RR_FLAGS_FORCEAPPLY, "A"),
                                  F(ICMP6_RR_FLAGS_SPECSITE, "S"),
                                  F(ICMP6_RR_FLAGS_PREVDONE, "P")));
		}
                ND_PRINT((ndo,"seg=%u,", rr6->rr_segnum));
                ND_PRINT((ndo,"maxdelay=%u", EXTRACT_16BITS(&rr6->rr_maxdelay)));
		if (rr6->rr_reserved)
			ND_PRINT((ndo,"rsvd=0x%x", EXTRACT_32BITS(&rr6->rr_reserved)));
		/*[*/
		ND_PRINT((ndo,"]"));
#undef F
	}

	if (rr6->rr_code == ICMP6_ROUTER_RENUMBERING_COMMAND) {
		match = (const struct rr_pco_match *)cp;
		cp = (const char *)(match + 1);

		ND_TCHECK(match->rpm_prefix);

		if (ndo->ndo_vflag > 1)
			ND_PRINT((ndo,"\n\t"));
		else
			ND_PRINT((ndo," "));
		ND_PRINT((ndo,"match("));	/*)*/
		switch (match->rpm_code) {
		case RPM_PCO_ADD:	ND_PRINT((ndo,"add")); break;
		case RPM_PCO_CHANGE:	ND_PRINT((ndo,"change")); break;
		case RPM_PCO_SETGLOBAL:	ND_PRINT((ndo,"setglobal")); break;
		default:		ND_PRINT((ndo,"#%u", match->rpm_code)); break;
		}

		if (ndo->ndo_vflag) {
			ND_PRINT((ndo,",ord=%u", match->rpm_ordinal));
			ND_PRINT((ndo,",min=%u", match->rpm_minlen));
			ND_PRINT((ndo,",max=%u", match->rpm_maxlen));
		}
		if (addrtostr6(&match->rpm_prefix, hbuf, sizeof(hbuf)))
			ND_PRINT((ndo,",%s/%u", hbuf, match->rpm_matchlen));
		else
			ND_PRINT((ndo,",?/%u", match->rpm_matchlen));
		/*(*/
		ND_PRINT((ndo,")"));

		n = match->rpm_len - 3;
		if (n % 4)
			goto trunc;
		n /= 4;
		while (n-- > 0) {
			use = (const struct rr_pco_use *)cp;
			cp = (const char *)(use + 1);

			ND_TCHECK(use->rpu_prefix);

			if (ndo->ndo_vflag > 1)
				ND_PRINT((ndo,"\n\t"));
			else
				ND_PRINT((ndo," "));
			ND_PRINT((ndo,"use("));	/*)*/
			if (use->rpu_flags) {
#define F(x, y)	((use->rpu_flags) & (x) ? (y) : "")
				ND_PRINT((ndo,"%s%s,",
                                          F(ICMP6_RR_PCOUSE_FLAGS_DECRVLTIME, "V"),
                                          F(ICMP6_RR_PCOUSE_FLAGS_DECRPLTIME, "P")));
#undef F
			}
			if (ndo->ndo_vflag) {
				ND_PRINT((ndo,"mask=0x%x,", use->rpu_ramask));
				ND_PRINT((ndo,"raflags=0x%x,", use->rpu_raflags));
				if (~use->rpu_vltime == 0)
					ND_PRINT((ndo,"vltime=infty,"));
				else
					ND_PRINT((ndo,"vltime=%u,",
                                                  EXTRACT_32BITS(&use->rpu_vltime)));
				if (~use->rpu_pltime == 0)
					ND_PRINT((ndo,"pltime=infty,"));
				else
					ND_PRINT((ndo,"pltime=%u,",
                                                  EXTRACT_32BITS(&use->rpu_pltime)));
			}
			if (addrtostr6(&use->rpu_prefix, hbuf, sizeof(hbuf)))
				ND_PRINT((ndo,"%s/%u/%u", hbuf, use->rpu_uselen,
                                          use->rpu_keeplen));
			else
				ND_PRINT((ndo,"?/%u/%u", use->rpu_uselen,
                                          use->rpu_keeplen));
			/*(*/
                        ND_PRINT((ndo,")"));
		}
	}

 	return;
 
 trunc:
	ND_PRINT((ndo, "%s", icmp6_tstr));
 }
