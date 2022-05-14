l2tp_ppp_discon_cc_print(netdissect_options *ndo, const u_char *dat, u_int length)
 {
 	const uint16_t *ptr = (const uint16_t *)dat;
 
	ND_PRINT((ndo, "%04x, ", EXTRACT_16BITS(ptr))); ptr++;	/* Disconnect Code */
	ND_PRINT((ndo, "%04x ",  EXTRACT_16BITS(ptr))); ptr++;	/* Control Protocol Number */
 	ND_PRINT((ndo, "%s", tok2str(l2tp_cc_direction2str,
			     "Direction-#%u", *((const u_char *)ptr++))));
 
	if (length > 5) {
 		ND_PRINT((ndo, " "));
		print_string(ndo, (const u_char *)ptr, length-5);
 	}
 }
