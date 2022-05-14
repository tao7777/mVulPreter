lmp_print_data_link_subobjs(netdissect_options *ndo, const u_char *obj_tptr,
                            int total_subobj_len, int offset)
{
    int hexdump = FALSE;
    int subobj_type, subobj_len;

    union { /* int to float conversion buffer */
        float f;
        uint32_t i;
     } bw;
 
     while (total_subobj_len > 0 && hexdump == FALSE ) {
 	subobj_type = EXTRACT_8BITS(obj_tptr + offset);
 	subobj_len  = EXTRACT_8BITS(obj_tptr + offset + 1);
 	ND_PRINT((ndo, "\n\t    Subobject, Type: %s (%u), Length: %u",
		tok2str(lmp_data_link_subobj,
			"Unknown",
			subobj_type),
			subobj_type,
			subobj_len));
	if (subobj_len < 4) {
	    ND_PRINT((ndo, " (too short)"));
	    break;
	}
	if ((subobj_len % 4) != 0) {
	    ND_PRINT((ndo, " (not a multiple of 4)"));
	    break;
	}
	if (total_subobj_len < subobj_len) {
	    ND_PRINT((ndo, " (goes past the end of the object)"));
	    break;
 	}
 	switch(subobj_type) {
 	case INT_SWITCHING_TYPE_SUBOBJ:
 	    ND_PRINT((ndo, "\n\t      Switching Type: %s (%u)",
 		tok2str(gmpls_switch_cap_values,
 			"Unknown",
 			EXTRACT_8BITS(obj_tptr + offset + 2)),
 			EXTRACT_8BITS(obj_tptr + offset + 2)));
 	    ND_PRINT((ndo, "\n\t      Encoding Type: %s (%u)",
 		tok2str(gmpls_encoding_values,
 			"Unknown",
			EXTRACT_8BITS(obj_tptr + offset + 3)),
			EXTRACT_8BITS(obj_tptr + offset + 3)));
	    ND_TCHECK_32BITS(obj_tptr + offset + 4);
 	    bw.i = EXTRACT_32BITS(obj_tptr+offset+4);
 	    ND_PRINT((ndo, "\n\t      Min Reservable Bandwidth: %.3f Mbps",
                 bw.f*8/1000000));
 	    bw.i = EXTRACT_32BITS(obj_tptr+offset+8);
 	    ND_PRINT((ndo, "\n\t      Max Reservable Bandwidth: %.3f Mbps",
                 bw.f*8/1000000));
 	    break;
 	case WAVELENGTH_SUBOBJ:
 	    ND_PRINT((ndo, "\n\t      Wavelength: %u",
 		EXTRACT_32BITS(obj_tptr+offset+4)));
 	    break;
	default:
	    /* Any Unknown Subobject ==> Exit loop */
	    hexdump=TRUE;
	    break;
	}
	total_subobj_len-=subobj_len;
	offset+=subobj_len;
    }
    return (hexdump);
trunc:
    return -1;
}
