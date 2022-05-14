l2tp_msgtype_print(netdissect_options *ndo, const u_char *dat)
 {
 	const uint16_t *ptr = (const uint16_t *)dat;
 
 	ND_PRINT((ndo, "%s", tok2str(l2tp_msgtype2str, "MSGTYPE-#%u",
 	    EXTRACT_16BITS(ptr))));
 }
