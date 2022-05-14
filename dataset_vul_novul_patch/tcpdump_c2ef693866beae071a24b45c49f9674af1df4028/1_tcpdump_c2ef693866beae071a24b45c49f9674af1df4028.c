 ikev1_n_print(netdissect_options *ndo, u_char tpay _U_,
 	      const struct isakmp_gen *ext, u_int item_len,
	      const u_char *ep, uint32_t phase, uint32_t doi0 _U_,
	      uint32_t proto0 _U_, int depth)
 {
 	const struct ikev1_pl_n *p;
 	struct ikev1_pl_n n;
	const u_char *cp;
	const u_char *ep2;
	uint32_t doi;
	uint32_t proto;
	static const char *notify_error_str[] = {
		NULL,				"INVALID-PAYLOAD-TYPE",
		"DOI-NOT-SUPPORTED",		"SITUATION-NOT-SUPPORTED",
		"INVALID-COOKIE",		"INVALID-MAJOR-VERSION",
		"INVALID-MINOR-VERSION",	"INVALID-EXCHANGE-TYPE",
		"INVALID-FLAGS",		"INVALID-MESSAGE-ID",
		"INVALID-PROTOCOL-ID",		"INVALID-SPI",
		"INVALID-TRANSFORM-ID",		"ATTRIBUTES-NOT-SUPPORTED",
		"NO-PROPOSAL-CHOSEN",		"BAD-PROPOSAL-SYNTAX",
		"PAYLOAD-MALFORMED",		"INVALID-KEY-INFORMATION",
		"INVALID-ID-INFORMATION",	"INVALID-CERT-ENCODING",
		"INVALID-CERTIFICATE",		"CERT-TYPE-UNSUPPORTED",
		"INVALID-CERT-AUTHORITY",	"INVALID-HASH-INFORMATION",
		"AUTHENTICATION-FAILED",	"INVALID-SIGNATURE",
		"ADDRESS-NOTIFICATION",		"NOTIFY-SA-LIFETIME",
		"CERTIFICATE-UNAVAILABLE",	"UNSUPPORTED-EXCHANGE-TYPE",
		"UNEQUAL-PAYLOAD-LENGTHS",
	};
	static const char *ipsec_notify_error_str[] = {
		"RESERVED",
	};
	static const char *notify_status_str[] = {
		"CONNECTED",
	};
	static const char *ipsec_notify_status_str[] = {
		"RESPONDER-LIFETIME",		"REPLAY-STATUS",
		"INITIAL-CONTACT",
	};
/* NOTE: these macro must be called with x in proper range */

/* 0 - 8191 */
#define NOTIFY_ERROR_STR(x) \
	STR_OR_ID((x), notify_error_str)

/* 8192 - 16383 */
#define IPSEC_NOTIFY_ERROR_STR(x) \
	STR_OR_ID((u_int)((x) - 8192), ipsec_notify_error_str)

/* 16384 - 24575 */
#define NOTIFY_STATUS_STR(x) \
	STR_OR_ID((u_int)((x) - 16384), notify_status_str)

/* 24576 - 32767 */
#define IPSEC_NOTIFY_STATUS_STR(x) \
	STR_OR_ID((u_int)((x) - 24576), ipsec_notify_status_str)

	ND_PRINT((ndo,"%s:", NPSTR(ISAKMP_NPTYPE_N)));

	p = (const struct ikev1_pl_n *)ext;
	ND_TCHECK(*p);
	UNALIGNED_MEMCPY(&n, ext, sizeof(n));
	doi = ntohl(n.doi);
	proto = n.prot_id;
	if (doi != 1) {
		ND_PRINT((ndo," doi=%d", doi));
		ND_PRINT((ndo," proto=%d", proto));
		if (ntohs(n.type) < 8192)
			ND_PRINT((ndo," type=%s", NOTIFY_ERROR_STR(ntohs(n.type))));
		else if (ntohs(n.type) < 16384)
			ND_PRINT((ndo," type=%s", numstr(ntohs(n.type))));
		else if (ntohs(n.type) < 24576)
			ND_PRINT((ndo," type=%s", NOTIFY_STATUS_STR(ntohs(n.type))));
		else
			ND_PRINT((ndo," type=%s", numstr(ntohs(n.type))));
		if (n.spi_size) {
			ND_PRINT((ndo," spi="));
			if (!rawprint(ndo, (const uint8_t *)(p + 1), n.spi_size))
				goto trunc;
		}
		return (const u_char *)(p + 1) + n.spi_size;
	}

	ND_PRINT((ndo," doi=ipsec"));
	ND_PRINT((ndo," proto=%s", PROTOIDSTR(proto)));
	if (ntohs(n.type) < 8192)
		ND_PRINT((ndo," type=%s", NOTIFY_ERROR_STR(ntohs(n.type))));
	else if (ntohs(n.type) < 16384)
		ND_PRINT((ndo," type=%s", IPSEC_NOTIFY_ERROR_STR(ntohs(n.type))));
	else if (ntohs(n.type) < 24576)
		ND_PRINT((ndo," type=%s", NOTIFY_STATUS_STR(ntohs(n.type))));
	else if (ntohs(n.type) < 32768)
		ND_PRINT((ndo," type=%s", IPSEC_NOTIFY_STATUS_STR(ntohs(n.type))));
	else
		ND_PRINT((ndo," type=%s", numstr(ntohs(n.type))));
	if (n.spi_size) {
		ND_PRINT((ndo," spi="));
		if (!rawprint(ndo, (const uint8_t *)(p + 1), n.spi_size))
			goto trunc;
	}

	cp = (const u_char *)(p + 1) + n.spi_size;
 	ep2 = (const u_char *)p + item_len;
 
 	if (cp < ep) {
		ND_PRINT((ndo," orig=("));
 		switch (ntohs(n.type)) {
 		case IPSECDOI_NTYPE_RESPONDER_LIFETIME:
 		    {
 			const struct attrmap *map = oakley_t_map;
 			size_t nmap = sizeof(oakley_t_map)/sizeof(oakley_t_map[0]);
 			while (cp < ep && cp < ep2) {
 				cp = ikev1_attrmap_print(ndo, cp,
 					(ep < ep2) ? ep : ep2, map, nmap);
 			}
 			break;
 		    }
 		case IPSECDOI_NTYPE_REPLAY_STATUS:
 			ND_PRINT((ndo,"replay detection %sabled",
 				  EXTRACT_32BITS(cp) ? "en" : "dis"));
			break;
		case ISAKMP_NTYPE_NO_PROPOSAL_CHOSEN:
			if (ikev1_sub_print(ndo, ISAKMP_NPTYPE_SA,
					    (const struct isakmp_gen *)cp, ep, phase, doi, proto,
					    depth) == NULL)
				return NULL;
 			break;
 		default:
			/* NULL is dummy */
			isakmp_print(ndo, cp,
				     item_len - sizeof(*p) - n.spi_size,
				     NULL);
 		}
		ND_PRINT((ndo,")"));
 	}
 	return (const u_char *)ext + item_len;
 trunc:
	ND_PRINT((ndo," [|%s]", NPSTR(ISAKMP_NPTYPE_N)));
	return NULL;
}
