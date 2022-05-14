dhcp6opt_print(netdissect_options *ndo,
               const u_char *cp, const u_char *ep)
{
	const struct dhcp6opt *dh6o;
	const u_char *tp;
	size_t i;
	uint16_t opttype;
	size_t optlen;
	uint8_t auth_proto;
	u_int authinfolen, authrealmlen;
	int remain_len;  /* Length of remaining options */
	int label_len;   /* Label length */
	uint16_t subopt_code;
	uint16_t subopt_len;

	if (cp == ep)
		return;
	while (cp < ep) {
		if (ep < cp + sizeof(*dh6o))
			goto trunc;
		dh6o = (const struct dhcp6opt *)cp;
		ND_TCHECK(*dh6o);
		optlen = EXTRACT_16BITS(&dh6o->dh6opt_len);
		if (ep < cp + sizeof(*dh6o) + optlen)
			goto trunc;
		opttype = EXTRACT_16BITS(&dh6o->dh6opt_type);
		ND_PRINT((ndo, " (%s", tok2str(dh6opt_str, "opt_%u", opttype)));
		ND_TCHECK2(*(cp + sizeof(*dh6o)), optlen);
		switch (opttype) {
		case DH6OPT_CLIENTID:
		case DH6OPT_SERVERID:
			if (optlen < 2) {
				/*(*/
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			switch (EXTRACT_16BITS(tp)) {
			case 1:
				if (optlen >= 2 + 6) {
					ND_PRINT((ndo, " hwaddr/time type %u time %u ",
					    EXTRACT_16BITS(&tp[2]),
					    EXTRACT_32BITS(&tp[4])));
					for (i = 8; i < optlen; i++)
						ND_PRINT((ndo, "%02x", tp[i]));
					/*(*/
					ND_PRINT((ndo, ")"));
				} else {
					/*(*/
					ND_PRINT((ndo, " ?)"));
				}
				break;
			case 2:
				if (optlen >= 2 + 8) {
					ND_PRINT((ndo, " vid "));
					for (i = 2; i < 2 + 8; i++)
						ND_PRINT((ndo, "%02x", tp[i]));
					/*(*/
					ND_PRINT((ndo, ")"));
				} else {
					/*(*/
					ND_PRINT((ndo, " ?)"));
				}
				break;
			case 3:
				if (optlen >= 2 + 2) {
					ND_PRINT((ndo, " hwaddr type %u ",
					    EXTRACT_16BITS(&tp[2])));
					for (i = 4; i < optlen; i++)
						ND_PRINT((ndo, "%02x", tp[i]));
					/*(*/
					ND_PRINT((ndo, ")"));
				} else {
					/*(*/
					ND_PRINT((ndo, " ?)"));
				}
				break;
			default:
				ND_PRINT((ndo, " type %d)", EXTRACT_16BITS(tp)));
				break;
			}
			break;
		case DH6OPT_IA_ADDR:
			if (optlen < 24) {
				/*(*/
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			ND_PRINT((ndo, " %s", ip6addr_string(ndo, &tp[0])));
			ND_PRINT((ndo, " pltime:%u vltime:%u",
			    EXTRACT_32BITS(&tp[16]),
			    EXTRACT_32BITS(&tp[20])));
			if (optlen > 24) {
				/* there are sub-options */
				dhcp6opt_print(ndo, tp + 24, tp + optlen);
			}
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_ORO:
		case DH6OPT_ERO:
			if (optlen % 2) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			for (i = 0; i < optlen; i += 2) {
				ND_PRINT((ndo, " %s",
				    tok2str(dh6opt_str, "opt_%u", EXTRACT_16BITS(&tp[i]))));
			}
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_PREFERENCE:
			if (optlen != 1) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			ND_PRINT((ndo, " %d)", *tp));
			break;
		case DH6OPT_ELAPSED_TIME:
			if (optlen != 2) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			ND_PRINT((ndo, " %d)", EXTRACT_16BITS(tp)));
			break;
		case DH6OPT_RELAY_MSG:
			ND_PRINT((ndo, " ("));
			tp = (const u_char *)(dh6o + 1);
			dhcp6_print(ndo, tp, optlen);
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_AUTH:
			if (optlen < 11) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			auth_proto = *tp;
			switch (auth_proto) {
			case DH6OPT_AUTHPROTO_DELAYED:
				ND_PRINT((ndo, " proto: delayed"));
				break;
			case DH6OPT_AUTHPROTO_RECONFIG:
				ND_PRINT((ndo, " proto: reconfigure"));
				break;
			default:
				ND_PRINT((ndo, " proto: %d", auth_proto));
				break;
			}
			tp++;
			switch (*tp) {
			case DH6OPT_AUTHALG_HMACMD5:
				/* XXX: may depend on the protocol */
				ND_PRINT((ndo, ", alg: HMAC-MD5"));
				break;
			default:
				ND_PRINT((ndo, ", alg: %d", *tp));
				break;
			}
			tp++;
			switch (*tp) {
			case DH6OPT_AUTHRDM_MONOCOUNTER:
				ND_PRINT((ndo, ", RDM: mono"));
				break;
			default:
				ND_PRINT((ndo, ", RDM: %d", *tp));
				break;
			}
			tp++;
			ND_PRINT((ndo, ", RD:"));
			for (i = 0; i < 4; i++, tp += 2)
				ND_PRINT((ndo, " %04x", EXTRACT_16BITS(tp)));

			/* protocol dependent part */
			authinfolen = optlen - 11;
			switch (auth_proto) {
			case DH6OPT_AUTHPROTO_DELAYED:
				if (authinfolen == 0)
					break;
				if (authinfolen < 20) {
					ND_PRINT((ndo, " ??"));
					break;
				}
				authrealmlen = authinfolen - 20;
				if (authrealmlen > 0) {
					ND_PRINT((ndo, ", realm: "));
				}
				for (i = 0; i < authrealmlen; i++, tp++)
					ND_PRINT((ndo, "%02x", *tp));
				ND_PRINT((ndo, ", key ID: %08x", EXTRACT_32BITS(tp)));
				tp += 4;
				ND_PRINT((ndo, ", HMAC-MD5:"));
				for (i = 0; i < 4; i++, tp+= 4)
					ND_PRINT((ndo, " %08x", EXTRACT_32BITS(tp)));
				break;
			case DH6OPT_AUTHPROTO_RECONFIG:
				if (authinfolen != 17) {
					ND_PRINT((ndo, " ??"));
					break;
				}
				switch (*tp++) {
				case DH6OPT_AUTHRECONFIG_KEY:
					ND_PRINT((ndo, " reconfig-key"));
					break;
				case DH6OPT_AUTHRECONFIG_HMACMD5:
					ND_PRINT((ndo, " type: HMAC-MD5"));
					break;
				default:
					ND_PRINT((ndo, " type: ??"));
					break;
				}
				ND_PRINT((ndo, " value:"));
				for (i = 0; i < 4; i++, tp+= 4)
					ND_PRINT((ndo, " %08x", EXTRACT_32BITS(tp)));
				break;
			default:
				ND_PRINT((ndo, " ??"));
				break;
			}

			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_RAPID_COMMIT: /* nothing todo */
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_INTERFACE_ID:
		case DH6OPT_SUBSCRIBER_ID:
			/*
			 * Since we cannot predict the encoding, print hex dump
			 * at most 10 characters.
			 */
			tp = (const u_char *)(dh6o + 1);
			ND_PRINT((ndo, " "));
			for (i = 0; i < optlen && i < 10; i++)
				ND_PRINT((ndo, "%02x", tp[i]));
 			ND_PRINT((ndo, "...)"));
 			break;
 		case DH6OPT_RECONF_MSG:
			if (optlen != 1) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
 			tp = (const u_char *)(dh6o + 1);
 			switch (*tp) {
 			case DH6_RENEW:
				ND_PRINT((ndo, " for renew)"));
				break;
			case DH6_INFORM_REQ:
				ND_PRINT((ndo, " for inf-req)"));
				break;
			default:
				ND_PRINT((ndo, " for ?\?\?(%02x))", *tp));
				break;
			}
			break;
		case DH6OPT_RECONF_ACCEPT: /* nothing todo */
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_SIP_SERVER_A:
		case DH6OPT_DNS_SERVERS:
		case DH6OPT_SNTP_SERVERS:
		case DH6OPT_NIS_SERVERS:
		case DH6OPT_NISP_SERVERS:
		case DH6OPT_BCMCS_SERVER_A:
		case DH6OPT_PANA_AGENT:
		case DH6OPT_LQ_CLIENT_LINK:
			if (optlen % 16) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			for (i = 0; i < optlen; i += 16)
				ND_PRINT((ndo, " %s", ip6addr_string(ndo, &tp[i])));
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_SIP_SERVER_D:
		case DH6OPT_DOMAIN_LIST:
			tp = (const u_char *)(dh6o + 1);
			while (tp < cp + sizeof(*dh6o) + optlen) {
				ND_PRINT((ndo, " "));
				if ((tp = ns_nprint(ndo, tp, cp + sizeof(*dh6o) + optlen)) == NULL)
					goto trunc;
			}
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_STATUS_CODE:
			if (optlen < 2) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			ND_PRINT((ndo, " %s)", dhcp6stcode(EXTRACT_16BITS(&tp[0]))));
			break;
		case DH6OPT_IA_NA:
		case DH6OPT_IA_PD:
			if (optlen < 12) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			ND_PRINT((ndo, " IAID:%u T1:%u T2:%u",
			    EXTRACT_32BITS(&tp[0]),
			    EXTRACT_32BITS(&tp[4]),
			    EXTRACT_32BITS(&tp[8])));
			if (optlen > 12) {
				/* there are sub-options */
				dhcp6opt_print(ndo, tp + 12, tp + optlen);
			}
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_IA_TA:
			if (optlen < 4) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			ND_PRINT((ndo, " IAID:%u", EXTRACT_32BITS(tp)));
			if (optlen > 4) {
				/* there are sub-options */
				dhcp6opt_print(ndo, tp + 4, tp + optlen);
			}
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_IA_PD_PREFIX:
			if (optlen < 25) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			ND_PRINT((ndo, " %s/%d", ip6addr_string(ndo, &tp[9]), tp[8]));
			ND_PRINT((ndo, " pltime:%u vltime:%u",
			    EXTRACT_32BITS(&tp[0]),
			    EXTRACT_32BITS(&tp[4])));
			if (optlen > 25) {
				/* there are sub-options */
				dhcp6opt_print(ndo, tp + 25, tp + optlen);
			}
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_LIFETIME:
		case DH6OPT_CLT_TIME:
			if (optlen != 4) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			ND_PRINT((ndo, " %d)", EXTRACT_32BITS(tp)));
			break;
		case DH6OPT_REMOTE_ID:
			if (optlen < 4) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			ND_PRINT((ndo, " %d ", EXTRACT_32BITS(tp)));
			/*
			 * Print hex dump first 10 characters.
			 */
			for (i = 4; i < optlen && i < 14; i++)
				ND_PRINT((ndo, "%02x", tp[i]));
			ND_PRINT((ndo, "...)"));
			break;
		case DH6OPT_LQ_QUERY:
			if (optlen < 17) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			switch (*tp) {
			case 1:
				ND_PRINT((ndo, " by-address"));
				break;
			case 2:
				ND_PRINT((ndo, " by-clientID"));
				break;
			default:
				ND_PRINT((ndo, " type_%d", (int)*tp));
				break;
			}
			ND_PRINT((ndo, " %s", ip6addr_string(ndo, &tp[1])));
			if (optlen > 17) {
				/* there are query-options */
				dhcp6opt_print(ndo, tp + 17, tp + optlen);
			}
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_CLIENT_DATA:
			tp = (const u_char *)(dh6o + 1);
			if (optlen > 0) {
				/* there are encapsulated options */
				dhcp6opt_print(ndo, tp, tp + optlen);
			}
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_LQ_RELAY_DATA:
			if (optlen < 16) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			ND_PRINT((ndo, " %s ", ip6addr_string(ndo, &tp[0])));
			/*
			 * Print hex dump first 10 characters.
			 */
			for (i = 16; i < optlen && i < 26; i++)
				ND_PRINT((ndo, "%02x", tp[i]));
			ND_PRINT((ndo, "...)"));
			break;
		case DH6OPT_NTP_SERVER:
			if (optlen < 4) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			while (tp < cp + sizeof(*dh6o) + optlen - 4) {
				subopt_code = EXTRACT_16BITS(tp);
				tp += 2;
				subopt_len = EXTRACT_16BITS(tp);
				tp += 2;
				if (tp + subopt_len > cp + sizeof(*dh6o) + optlen)
					goto trunc;
				ND_PRINT((ndo, " subopt:%d", subopt_code));
				switch (subopt_code) {
				case DH6OPT_NTP_SUBOPTION_SRV_ADDR:
				case DH6OPT_NTP_SUBOPTION_MC_ADDR:
					if (subopt_len != 16) {
						ND_PRINT((ndo, " ?"));
						break;
					}
					ND_PRINT((ndo, " %s", ip6addr_string(ndo, &tp[0])));
					break;
				case DH6OPT_NTP_SUBOPTION_SRV_FQDN:
					ND_PRINT((ndo, " "));
					if (ns_nprint(ndo, tp, tp + subopt_len) == NULL)
						goto trunc;
					break;
				default:
					ND_PRINT((ndo, " ?"));
					break;
				}
				tp += subopt_len;
			}
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_AFTR_NAME:
			if (optlen < 3) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			remain_len = optlen;
			ND_PRINT((ndo, " "));
			/* Encoding is described in section 3.1 of RFC 1035 */
			while (remain_len && *tp) {
				label_len =  *tp++;
				if (label_len < remain_len - 1) {
					(void)fn_printn(ndo, tp, label_len, NULL);
					tp += label_len;
					remain_len -= (label_len + 1);
					if(*tp) ND_PRINT((ndo, "."));
				} else {
					ND_PRINT((ndo, " ?"));
					break;
				}
			}
			ND_PRINT((ndo, ")"));
			break;
		case DH6OPT_NEW_POSIX_TIMEZONE: /* all three of these options */
		case DH6OPT_NEW_TZDB_TIMEZONE:	/* are encoded similarly */
		case DH6OPT_MUDURL:		/* although GMT might not work */
		        if (optlen < 5) {
				ND_PRINT((ndo, " ?)"));
				break;
			}
			tp = (const u_char *)(dh6o + 1);
			ND_PRINT((ndo, "="));
			(void)fn_printn(ndo, tp, (u_int)optlen, NULL);
			ND_PRINT((ndo, ")"));
			break;

		default:
			ND_PRINT((ndo, ")"));
			break;
		}

		cp += sizeof(*dh6o) + optlen;
	}
	return;

trunc:
	ND_PRINT((ndo, "[|dhcp6ext]"));
}
