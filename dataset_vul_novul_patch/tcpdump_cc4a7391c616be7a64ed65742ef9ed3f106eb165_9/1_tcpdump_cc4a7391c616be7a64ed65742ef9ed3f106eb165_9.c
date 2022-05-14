l2tp_proxy_auth_id_print(netdissect_options *ndo, const u_char *dat)
 {
 	const uint16_t *ptr = (const uint16_t *)dat;
 
 	ND_PRINT((ndo, "%u", EXTRACT_16BITS(ptr) & L2TP_PROXY_AUTH_ID_MASK));
 }
