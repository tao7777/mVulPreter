rpki_rtr_pdu_print (netdissect_options *ndo, const u_char *tptr, u_int indent)
 {
     const rpki_rtr_pdu *pdu_header;
     u_int pdu_type, pdu_len, hexdump;
     const u_char *msg;
 
     pdu_header = (const rpki_rtr_pdu *)tptr;
     pdu_type = pdu_header->pdu_type;
     pdu_len = EXTRACT_32BITS(pdu_header->length);
    ND_TCHECK2(*tptr, pdu_len);
     hexdump = FALSE;
 
     ND_PRINT((ndo, "%sRPKI-RTRv%u, %s PDU (%u), length: %u",
 	   indent_string(8),
 	   pdu_header->version,
 	   tok2str(rpki_rtr_pdu_values, "Unknown", pdu_type),
 	   pdu_type, pdu_len));
 
     switch (pdu_type) {
 
	/*
	 * The following PDUs share the message format.
	 */
     case RPKI_RTR_SERIAL_NOTIFY_PDU:
     case RPKI_RTR_SERIAL_QUERY_PDU:
     case RPKI_RTR_END_OF_DATA_PDU:
         msg = (const u_char *)(pdu_header + 1);
 	ND_PRINT((ndo, "%sSession ID: 0x%04x, Serial: %u",
 	       indent_string(indent+2),
	       EXTRACT_16BITS(pdu_header->u.session_id),
	       EXTRACT_32BITS(msg)));
	break;

	/*
	 * The following PDUs share the message format.
 	 */
     case RPKI_RTR_RESET_QUERY_PDU:
     case RPKI_RTR_CACHE_RESET_PDU:
 
 	/*
 	 * Zero payload PDUs.
 	 */
 	break;
 
     case RPKI_RTR_CACHE_RESPONSE_PDU:
 	ND_PRINT((ndo, "%sSession ID: 0x%04x",
 	       indent_string(indent+2),
 	       EXTRACT_16BITS(pdu_header->u.session_id)));
	break;

    case RPKI_RTR_IPV4_PREFIX_PDU:
 	{
 	    const rpki_rtr_pdu_ipv4_prefix *pdu;
 
 	    pdu = (const rpki_rtr_pdu_ipv4_prefix *)tptr;
 	    ND_PRINT((ndo, "%sIPv4 Prefix %s/%u-%u, origin-as %u, flags 0x%02x",
 		   indent_string(indent+2),
		   ipaddr_string(ndo, pdu->prefix),
		   pdu->prefix_length, pdu->max_length,
		   EXTRACT_32BITS(pdu->as), pdu->flags));
	}
	break;

    case RPKI_RTR_IPV6_PREFIX_PDU:
 	{
 	    const rpki_rtr_pdu_ipv6_prefix *pdu;
 
 	    pdu = (const rpki_rtr_pdu_ipv6_prefix *)tptr;
 	    ND_PRINT((ndo, "%sIPv6 Prefix %s/%u-%u, origin-as %u, flags 0x%02x",
 		   indent_string(indent+2),
		   ip6addr_string(ndo, pdu->prefix),
		   pdu->prefix_length, pdu->max_length,
		   EXTRACT_32BITS(pdu->as), pdu->flags));
	}
	break;

    case RPKI_RTR_ERROR_REPORT_PDU:
	{
 	    const rpki_rtr_pdu_error_report *pdu;
 	    u_int encapsulated_pdu_length, text_length, tlen, error_code;
 
 	    pdu = (const rpki_rtr_pdu_error_report *)tptr;
 	    encapsulated_pdu_length = EXTRACT_32BITS(pdu->encapsulated_pdu_length);
	    ND_TCHECK2(*tptr, encapsulated_pdu_length);
	    tlen = pdu_len;
 
 	    error_code = EXTRACT_16BITS(pdu->pdu_header.u.error_code);
 	    ND_PRINT((ndo, "%sError code: %s (%u), Encapsulated PDU length: %u",
 		   indent_string(indent+2),
 		   tok2str(rpki_rtr_error_codes, "Unknown", error_code),
 		   error_code, encapsulated_pdu_length));
 
	    tptr += sizeof(*pdu);
	    tlen -= sizeof(*pdu);
	    /*
	     * Recurse if there is an encapsulated PDU.
	     */
	    if (encapsulated_pdu_length &&
		(encapsulated_pdu_length <= tlen)) {
		ND_PRINT((ndo, "%s-----encapsulated PDU-----", indent_string(indent+4)));
		if (rpki_rtr_pdu_print(ndo, tptr, indent+2))
			goto trunc;
 	    }
 
	    tptr += encapsulated_pdu_length;
	    tlen -= encapsulated_pdu_length;
 
 	    /*
 	     * Extract, trail-zero and print the Error message.
 	     */
	    text_length = 0;
	    if (tlen > 4) {
		text_length = EXTRACT_32BITS(tptr);
		tptr += 4;
		tlen -= 4;
	    }
	    ND_TCHECK2(*tptr, text_length);
	    if (text_length && (text_length <= tlen )) {
 		ND_PRINT((ndo, "%sError text: ", indent_string(indent+2)));
		if (fn_printn(ndo, tptr, text_length, ndo->ndo_snapend))
 			goto trunc;
 	    }
 	}
 	break;
 
     default:
 
 	/*
 	 * Unknown data, please hexdump.
	 */
	hexdump = TRUE;
    }

    /* do we also want to see a hex dump ? */
     if (ndo->ndo_vflag > 1 || (ndo->ndo_vflag && hexdump)) {
 	print_unknown_data(ndo,tptr,"\n\t  ", pdu_len);
     }
    return 0;
 
 trunc:
    return 1;
 }
