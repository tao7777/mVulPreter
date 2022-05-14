l2tp_proto_ver_print(netdissect_options *ndo, const uint16_t *dat)
l2tp_proto_ver_print(netdissect_options *ndo, const uint16_t *dat, u_int length)
 {
	if (length < 2) {
		ND_PRINT((ndo, "AVP too short"));
		return;
	}
 	ND_PRINT((ndo, "%u.%u", (EXTRACT_16BITS(dat) >> 8),
 	    (EXTRACT_16BITS(dat) & 0xff)));
 }
