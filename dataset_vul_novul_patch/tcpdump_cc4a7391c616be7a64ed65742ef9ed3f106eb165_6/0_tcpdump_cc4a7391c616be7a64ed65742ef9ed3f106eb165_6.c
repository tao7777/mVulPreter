l2tp_msgtype_print(netdissect_options *ndo, const u_char *dat)
l2tp_msgtype_print(netdissect_options *ndo, const u_char *dat, u_int length)
 {
 	const uint16_t *ptr = (const uint16_t *)dat;
 
	if (length < 2) {
		ND_PRINT((ndo, "AVP too short"));
		return;
	}
 	ND_PRINT((ndo, "%s", tok2str(l2tp_msgtype2str, "MSGTYPE-#%u",
 	    EXTRACT_16BITS(ptr))));
 }
