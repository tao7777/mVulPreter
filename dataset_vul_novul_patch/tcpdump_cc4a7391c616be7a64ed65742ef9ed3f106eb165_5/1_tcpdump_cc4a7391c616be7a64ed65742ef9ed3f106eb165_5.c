l2tp_framing_type_print(netdissect_options *ndo, const u_char *dat)
 {
 	const uint32_t *ptr = (const uint32_t *)dat;
 
 	if (EXTRACT_32BITS(ptr) &  L2TP_FRAMING_TYPE_ASYNC_MASK) {
 		ND_PRINT((ndo, "A"));
 	}
	if (EXTRACT_32BITS(ptr) &  L2TP_FRAMING_TYPE_SYNC_MASK) {
		ND_PRINT((ndo, "S"));
	}
}
