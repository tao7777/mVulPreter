ikev1_t_print(netdissect_options *ndo, u_char tpay _U_,
	      const struct isakmp_gen *ext, u_int item_len,
	      const u_char *ep, uint32_t phase _U_, uint32_t doi _U_,
	      uint32_t proto, int depth _U_)
{
	const struct ikev1_pl_t *p;
	struct ikev1_pl_t t;
	const u_char *cp;
	const char *idstr;
	const struct attrmap *map;
	size_t nmap;
	const u_char *ep2;

	ND_PRINT((ndo,"%s:", NPSTR(ISAKMP_NPTYPE_T)));

	p = (const struct ikev1_pl_t *)ext;
	ND_TCHECK(*p);
	UNALIGNED_MEMCPY(&t, ext, sizeof(t));

	switch (proto) {
	case 1:
		idstr = STR_OR_ID(t.t_id, ikev1_p_map);
		map = oakley_t_map;
		nmap = sizeof(oakley_t_map)/sizeof(oakley_t_map[0]);
		break;
	case 2:
		idstr = STR_OR_ID(t.t_id, ah_p_map);
		map = ipsec_t_map;
		nmap = sizeof(ipsec_t_map)/sizeof(ipsec_t_map[0]);
		break;
	case 3:
		idstr = STR_OR_ID(t.t_id, esp_p_map);
		map = ipsec_t_map;
		nmap = sizeof(ipsec_t_map)/sizeof(ipsec_t_map[0]);
		break;
	case 4:
		idstr = STR_OR_ID(t.t_id, ipcomp_p_map);
		map = ipsec_t_map;
		nmap = sizeof(ipsec_t_map)/sizeof(ipsec_t_map[0]);
		break;
	default:
		idstr = NULL;
		map = NULL;
		nmap = 0;
		break;
	}

	if (idstr)
		ND_PRINT((ndo," #%d id=%s ", t.t_no, idstr));
	else
		ND_PRINT((ndo," #%d id=%d ", t.t_no, t.t_id));
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
