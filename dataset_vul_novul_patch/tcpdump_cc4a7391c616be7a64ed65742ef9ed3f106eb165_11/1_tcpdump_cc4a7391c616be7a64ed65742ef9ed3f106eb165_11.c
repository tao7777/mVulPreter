 l2tp_q931_cc_print(netdissect_options *ndo, const u_char *dat, u_int length)
 {
 	print_16bits_val(ndo, (const uint16_t *)dat);
 	ND_PRINT((ndo, ", %02x", dat[2]));
	if (length > 3) {
 		ND_PRINT((ndo, " "));
		print_string(ndo, dat+3, length-3);
 	}
 }
