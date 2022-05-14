l2tp_proxy_auth_type_print(netdissect_options *ndo, const u_char *dat)
 {
 	const uint16_t *ptr = (const uint16_t *)dat;
 
 	ND_PRINT((ndo, "%s", tok2str(l2tp_authentype2str,
 			     "AuthType-#%u", EXTRACT_16BITS(ptr))));
 }
