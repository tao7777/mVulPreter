l2tp_call_errors_print(netdissect_options *ndo, const u_char *dat)
 {
 	const uint16_t *ptr = (const uint16_t *)dat;
 	uint16_t val_h, val_l;
 
 	ptr++;		/* skip "Reserved" */
 
	val_h = EXTRACT_16BITS(ptr); ptr++;
	val_l = EXTRACT_16BITS(ptr); ptr++;
 	ND_PRINT((ndo, "CRCErr=%u ", (val_h<<16) + val_l));
 
	val_h = EXTRACT_16BITS(ptr); ptr++;
	val_l = EXTRACT_16BITS(ptr); ptr++;
 	ND_PRINT((ndo, "FrameErr=%u ", (val_h<<16) + val_l));
 
	val_h = EXTRACT_16BITS(ptr); ptr++;
	val_l = EXTRACT_16BITS(ptr); ptr++;
 	ND_PRINT((ndo, "HardOver=%u ", (val_h<<16) + val_l));
 
	val_h = EXTRACT_16BITS(ptr); ptr++;
	val_l = EXTRACT_16BITS(ptr); ptr++;
 	ND_PRINT((ndo, "BufOver=%u ", (val_h<<16) + val_l));
 
	val_h = EXTRACT_16BITS(ptr); ptr++;
	val_l = EXTRACT_16BITS(ptr); ptr++;
 	ND_PRINT((ndo, "Timeout=%u ", (val_h<<16) + val_l));
 
 	val_h = EXTRACT_16BITS(ptr); ptr++;
 	val_l = EXTRACT_16BITS(ptr); ptr++;
 	ND_PRINT((ndo, "AlignErr=%u ", (val_h<<16) + val_l));
 }
