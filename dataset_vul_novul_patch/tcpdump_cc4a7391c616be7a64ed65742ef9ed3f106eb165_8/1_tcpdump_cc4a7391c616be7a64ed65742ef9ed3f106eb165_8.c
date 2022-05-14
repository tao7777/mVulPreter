l2tp_proto_ver_print(netdissect_options *ndo, const uint16_t *dat)
 {
 	ND_PRINT((ndo, "%u.%u", (EXTRACT_16BITS(dat) >> 8),
 	    (EXTRACT_16BITS(dat) & 0xff)));
 }
