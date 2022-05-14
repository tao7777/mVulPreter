l2tp_result_code_print(netdissect_options *ndo, const u_char *dat, u_int length)
 {
 	const uint16_t *ptr = (const uint16_t *)dat;
 
	/* Result Code */
	if (length < 2) {
		ND_PRINT((ndo, "AVP too short"));
		return;
 	}
	ND_PRINT((ndo, "%u", EXTRACT_16BITS(ptr)));
	ptr++;
	length -= 2;

	/* Error Code (opt) */
	if (length == 0)
		return;
	if (length < 2) {
		ND_PRINT((ndo, " AVP too short"));
		return;
 	}
	ND_PRINT((ndo, "/%u", EXTRACT_16BITS(ptr)));
	ptr++;
	length -= 2;

	/* Error Message (opt) */
	if (length == 0)
		return;
	ND_PRINT((ndo, " "));
	print_string(ndo, (const u_char *)ptr, length);
 }
