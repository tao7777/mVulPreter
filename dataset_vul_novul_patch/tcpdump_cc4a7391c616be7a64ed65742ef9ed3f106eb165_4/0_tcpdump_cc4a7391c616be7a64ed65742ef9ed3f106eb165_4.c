l2tp_framing_cap_print(netdissect_options *ndo, const u_char *dat)
l2tp_framing_cap_print(netdissect_options *ndo, const u_char *dat, u_int length)
 {
 	const uint32_t *ptr = (const uint32_t *)dat;
 
	if (length < 4) {
		ND_PRINT((ndo, "AVP too short"));
		return;
	}
 	if (EXTRACT_32BITS(ptr) &  L2TP_FRAMING_CAP_ASYNC_MASK) {
 		ND_PRINT((ndo, "A"));
 	}
	if (EXTRACT_32BITS(ptr) &  L2TP_FRAMING_CAP_SYNC_MASK) {
		ND_PRINT((ndo, "S"));
	}
 }
