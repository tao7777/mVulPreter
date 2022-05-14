l2tp_ppp_discon_cc_print(netdissect_options *ndo, const u_char *dat, u_int length)
 {
 	const uint16_t *ptr = (const uint16_t *)dat;
 
	if (length < 5) {
		ND_PRINT((ndo, "AVP too short"));
		return;
	}
	/* Disconnect Code */
	ND_PRINT((ndo, "%04x, ", EXTRACT_16BITS(dat)));
	dat += 2;
	length -= 2;
	/* Control Protocol Number */
	ND_PRINT((ndo, "%04x ",  EXTRACT_16BITS(dat)));
	dat += 2;
	length -= 2;
	/* Direction */
 	ND_PRINT((ndo, "%s", tok2str(l2tp_cc_direction2str,
			     "Direction-#%u", EXTRACT_8BITS(ptr))));
	ptr++;
	length--;
 
	if (length != 0) {
 		ND_PRINT((ndo, " "));
		print_string(ndo, (const u_char *)ptr, length);
 	}
 }
