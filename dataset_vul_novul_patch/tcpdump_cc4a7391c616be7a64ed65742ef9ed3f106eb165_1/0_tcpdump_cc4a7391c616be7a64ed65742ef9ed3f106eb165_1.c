l2tp_bearer_cap_print(netdissect_options *ndo, const u_char *dat)
l2tp_bearer_cap_print(netdissect_options *ndo, const u_char *dat, u_int length)
 {
 	const uint32_t *ptr = (const uint32_t *)dat;
 
	if (length < 4) {
		ND_PRINT((ndo, "AVP too short"));
		return;
	}
 	if (EXTRACT_32BITS(ptr) &  L2TP_BEARER_CAP_ANALOG_MASK) {
 		ND_PRINT((ndo, "A"));
 	}
	if (EXTRACT_32BITS(ptr) &  L2TP_BEARER_CAP_DIGITAL_MASK) {
		ND_PRINT((ndo, "D"));
	}
}
