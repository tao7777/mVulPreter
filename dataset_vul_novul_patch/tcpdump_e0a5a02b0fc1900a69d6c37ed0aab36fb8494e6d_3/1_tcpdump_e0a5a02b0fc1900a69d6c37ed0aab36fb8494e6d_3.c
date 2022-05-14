ikev2_t_print(netdissect_options *ndo, int tcount,
	      const struct isakmp_gen *ext, u_int item_len,
	      const u_char *ep)
{
	const struct ikev2_t *p;
	struct ikev2_t t;
	uint16_t  t_id;
	const u_char *cp;
	const char *idstr;
	const struct attrmap *map;
	size_t nmap;
	const u_char *ep2;

	p = (const struct ikev2_t *)ext;
	ND_TCHECK(*p);
	UNALIGNED_MEMCPY(&t, ext, sizeof(t));
	ikev2_pay_print(ndo, NPSTR(ISAKMP_NPTYPE_T), t.h.critical);

	t_id = ntohs(t.t_id);

	map = NULL;
	nmap = 0;

	switch (t.t_type) {
	case IV2_T_ENCR:
		idstr = STR_OR_ID(t_id, esp_p_map);
		map = encr_t_map;
		nmap = sizeof(encr_t_map)/sizeof(encr_t_map[0]);
		break;

	case IV2_T_PRF:
		idstr = STR_OR_ID(t_id, prf_p_map);
		break;

	case IV2_T_INTEG:
		idstr = STR_OR_ID(t_id, integ_p_map);
		break;

	case IV2_T_DH:
		idstr = STR_OR_ID(t_id, dh_p_map);
		break;

	case IV2_T_ESN:
		idstr = STR_OR_ID(t_id, esn_p_map);
		break;

	default:
		idstr = NULL;
		break;
	}

	if (idstr)
		ND_PRINT((ndo," #%u type=%s id=%s ", tcount,
			  STR_OR_ID(t.t_type, ikev2_t_type_map),
			  idstr));
	else
		ND_PRINT((ndo," #%u type=%s id=%u ", tcount,
			  STR_OR_ID(t.t_type, ikev2_t_type_map),
			  t.t_id));
	cp = (const u_char *)(p + 1);
 	ep2 = (const u_char *)p + item_len;
 	while (cp < ep && cp < ep2) {
 		if (map && nmap) {
			cp = ikev1_attrmap_print(ndo, cp, (ep < ep2) ? ep : ep2,
				map, nmap);
 		} else
			cp = ikev1_attr_print(ndo, cp, (ep < ep2) ? ep : ep2);
 	}
 	if (ep < ep2)
 		ND_PRINT((ndo,"..."));
	return cp;
trunc:
	ND_PRINT((ndo," [|%s]", NPSTR(ISAKMP_NPTYPE_T)));
	return NULL;
}
