icmp6_nodeinfo_print(netdissect_options *ndo, u_int icmp6len, const u_char *bp, const u_char *ep)
{
	const struct icmp6_nodeinfo *ni6;
	const struct icmp6_hdr *dp;
	const u_char *cp;
	size_t siz, i;
	int needcomma;

	if (ep < bp)
		return;
	dp = (const struct icmp6_hdr *)bp;
	ni6 = (const struct icmp6_nodeinfo *)bp;
	siz = ep - bp;

	switch (ni6->ni_type) {
	case ICMP6_NI_QUERY:
		if (siz == sizeof(*dp) + 4) {
			/* KAME who-are-you */
			ND_PRINT((ndo," who-are-you request"));
			break;
		}
		ND_PRINT((ndo," node information query"));

		ND_TCHECK2(*dp, sizeof(*ni6));
		ni6 = (const struct icmp6_nodeinfo *)dp;
		ND_PRINT((ndo," ("));	/*)*/
		switch (EXTRACT_16BITS(&ni6->ni_qtype)) {
		case NI_QTYPE_NOOP:
			ND_PRINT((ndo,"noop"));
			break;
		case NI_QTYPE_SUPTYPES:
			ND_PRINT((ndo,"supported qtypes"));
			i = EXTRACT_16BITS(&ni6->ni_flags);
			if (i)
				ND_PRINT((ndo," [%s]", (i & 0x01) ? "C" : ""));
			break;
		case NI_QTYPE_FQDN:
			ND_PRINT((ndo,"DNS name"));
			break;
		case NI_QTYPE_NODEADDR:
			ND_PRINT((ndo,"node addresses"));
			i = ni6->ni_flags;
			if (!i)
				break;
			/* NI_NODEADDR_FLAG_TRUNCATE undefined for query */
			ND_PRINT((ndo," [%s%s%s%s%s%s]",
			    (i & NI_NODEADDR_FLAG_ANYCAST) ? "a" : "",
			    (i & NI_NODEADDR_FLAG_GLOBAL) ? "G" : "",
			    (i & NI_NODEADDR_FLAG_SITELOCAL) ? "S" : "",
			    (i & NI_NODEADDR_FLAG_LINKLOCAL) ? "L" : "",
			    (i & NI_NODEADDR_FLAG_COMPAT) ? "C" : "",
			    (i & NI_NODEADDR_FLAG_ALL) ? "A" : ""));
			break;
		default:
			ND_PRINT((ndo,"unknown"));
			break;
		}

		if (ni6->ni_qtype == NI_QTYPE_NOOP ||
		    ni6->ni_qtype == NI_QTYPE_SUPTYPES) {
			if (siz != sizeof(*ni6))
				if (ndo->ndo_vflag)
					ND_PRINT((ndo,", invalid len"));
			/*(*/
			ND_PRINT((ndo,")"));
			break;
		}


		/* XXX backward compat, icmp-name-lookup-03 */
		if (siz == sizeof(*ni6)) {
			ND_PRINT((ndo,", 03 draft"));
			/*(*/
			ND_PRINT((ndo,")"));
			break;
		}

		switch (ni6->ni_code) {
		case ICMP6_NI_SUBJ_IPV6:
			if (!ND_TTEST2(*dp,
			    sizeof(*ni6) + sizeof(struct in6_addr)))
				break;
			if (siz != sizeof(*ni6) + sizeof(struct in6_addr)) {
				if (ndo->ndo_vflag)
					ND_PRINT((ndo,", invalid subject len"));
				break;
			}
			ND_PRINT((ndo,", subject=%s",
                                  ip6addr_string(ndo, ni6 + 1)));
			break;
		case ICMP6_NI_SUBJ_FQDN:
			ND_PRINT((ndo,", subject=DNS name"));
			cp = (const u_char *)(ni6 + 1);
			if (cp[0] == ep - cp - 1) {
				/* icmp-name-lookup-03, pascal string */
				if (ndo->ndo_vflag)
					ND_PRINT((ndo,", 03 draft"));
				cp++;
				ND_PRINT((ndo,", \""));
				while (cp < ep) {
					safeputchar(ndo, *cp);
					cp++;
				}
				ND_PRINT((ndo,"\""));
			} else
				dnsname_print(ndo, cp, ep);
			break;
		case ICMP6_NI_SUBJ_IPV4:
			if (!ND_TTEST2(*dp, sizeof(*ni6) + sizeof(struct in_addr)))
				break;
			if (siz != sizeof(*ni6) + sizeof(struct in_addr)) {
				if (ndo->ndo_vflag)
					ND_PRINT((ndo,", invalid subject len"));
				break;
			}
			ND_PRINT((ndo,", subject=%s",
                                  ipaddr_string(ndo, ni6 + 1)));
			break;
		default:
			ND_PRINT((ndo,", unknown subject"));
			break;
		}

		/*(*/
		ND_PRINT((ndo,")"));
		break;

	case ICMP6_NI_REPLY:
		if (icmp6len > siz) {
			ND_PRINT((ndo,"[|icmp6: node information reply]"));
			break;
		}
 
 		needcomma = 0;
 
		ND_TCHECK2(*dp, sizeof(*ni6));
 		ni6 = (const struct icmp6_nodeinfo *)dp;
 		ND_PRINT((ndo," node information reply"));
 		ND_PRINT((ndo," ("));	/*)*/
		switch (ni6->ni_code) {
		case ICMP6_NI_SUCCESS:
			if (ndo->ndo_vflag) {
				ND_PRINT((ndo,"success"));
				needcomma++;
			}
			break;
		case ICMP6_NI_REFUSED:
			ND_PRINT((ndo,"refused"));
			needcomma++;
			if (siz != sizeof(*ni6))
				if (ndo->ndo_vflag)
					ND_PRINT((ndo,", invalid length"));
			break;
		case ICMP6_NI_UNKNOWN:
			ND_PRINT((ndo,"unknown"));
			needcomma++;
			if (siz != sizeof(*ni6))
				if (ndo->ndo_vflag)
					ND_PRINT((ndo,", invalid length"));
			break;
		}

		if (ni6->ni_code != ICMP6_NI_SUCCESS) {
			/*(*/
			ND_PRINT((ndo,")"));
			break;
		}

		switch (EXTRACT_16BITS(&ni6->ni_qtype)) {
		case NI_QTYPE_NOOP:
			if (needcomma)
				ND_PRINT((ndo,", "));
			ND_PRINT((ndo,"noop"));
			if (siz != sizeof(*ni6))
				if (ndo->ndo_vflag)
					ND_PRINT((ndo,", invalid length"));
			break;
		case NI_QTYPE_SUPTYPES:
			if (needcomma)
				ND_PRINT((ndo,", "));
			ND_PRINT((ndo,"supported qtypes"));
			i = EXTRACT_16BITS(&ni6->ni_flags);
			if (i)
				ND_PRINT((ndo," [%s]", (i & 0x01) ? "C" : ""));
			break;
		case NI_QTYPE_FQDN:
			if (needcomma)
 				ND_PRINT((ndo,", "));
 			ND_PRINT((ndo,"DNS name"));
 			cp = (const u_char *)(ni6 + 1) + 4;
			ND_TCHECK(cp[0]);
 			if (cp[0] == ep - cp - 1) {
 				/* icmp-name-lookup-03, pascal string */
 				if (ndo->ndo_vflag)
					ND_PRINT((ndo,", 03 draft"));
				cp++;
				ND_PRINT((ndo,", \""));
				while (cp < ep) {
					safeputchar(ndo, *cp);
					cp++;
				}
				ND_PRINT((ndo,"\""));
			} else
				dnsname_print(ndo, cp, ep);
			if ((EXTRACT_16BITS(&ni6->ni_flags) & 0x01) != 0)
				ND_PRINT((ndo," [TTL=%u]", EXTRACT_32BITS(ni6 + 1)));
			break;
		case NI_QTYPE_NODEADDR:
			if (needcomma)
				ND_PRINT((ndo,", "));
			ND_PRINT((ndo,"node addresses"));
			i = sizeof(*ni6);
			while (i < siz) {
				if (i + sizeof(struct in6_addr) + sizeof(int32_t) > siz)
					break;
				ND_PRINT((ndo," %s", ip6addr_string(ndo, bp + i)));
				i += sizeof(struct in6_addr);
				ND_PRINT((ndo,"(%d)", (int32_t)EXTRACT_32BITS(bp + i)));
				i += sizeof(int32_t);
			}
			i = ni6->ni_flags;
			if (!i)
				break;
			ND_PRINT((ndo," [%s%s%s%s%s%s%s]",
                                  (i & NI_NODEADDR_FLAG_ANYCAST) ? "a" : "",
                                  (i & NI_NODEADDR_FLAG_GLOBAL) ? "G" : "",
                                  (i & NI_NODEADDR_FLAG_SITELOCAL) ? "S" : "",
                                  (i & NI_NODEADDR_FLAG_LINKLOCAL) ? "L" : "",
                                  (i & NI_NODEADDR_FLAG_COMPAT) ? "C" : "",
                                  (i & NI_NODEADDR_FLAG_ALL) ? "A" : "",
                                  (i & NI_NODEADDR_FLAG_TRUNCATE) ? "T" : ""));
			break;
		default:
			if (needcomma)
				ND_PRINT((ndo,", "));
			ND_PRINT((ndo,"unknown"));
			break;
		}

		/*(*/
		ND_PRINT((ndo,")"));
		break;
	}
	return;

trunc:
	ND_PRINT((ndo, "[|icmp6]"));
}
