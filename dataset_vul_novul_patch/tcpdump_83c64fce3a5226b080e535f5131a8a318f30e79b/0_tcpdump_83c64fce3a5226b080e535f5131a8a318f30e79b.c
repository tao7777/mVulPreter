rpki_rtr_pdu_print (netdissect_options *ndo, const u_char *tptr, u_int indent)
static u_int
rpki_rtr_pdu_print (netdissect_options *ndo, const u_char *tptr, const u_int len,
	const u_char recurse, const u_int indent)
 {
     const rpki_rtr_pdu *pdu_header;
     u_int pdu_type, pdu_len, hexdump;
     const u_char *msg;
 
    /* Protocol Version */
    ND_TCHECK_8BITS(tptr);
    if (*tptr != 0) {
	/* Skip the rest of the input buffer because even if this is
	 * a well-formed PDU of a future RPKI-Router protocol version
	 * followed by a well-formed PDU of RPKI-Router protocol
	 * version 0, there is no way to know exactly how to skip the
	 * current PDU.
	 */
	ND_PRINT((ndo, "%sRPKI-RTRv%u (unknown)", indent_string(8), *tptr));
	return len;
    }
    if (len < sizeof(rpki_rtr_pdu)) {
	ND_PRINT((ndo, "(%u bytes is too few to decode)", len));
	goto invalid;
    }
    ND_TCHECK2(*tptr, sizeof(rpki_rtr_pdu));
     pdu_header = (const rpki_rtr_pdu *)tptr;
     pdu_type = pdu_header->pdu_type;
     pdu_len = EXTRACT_32BITS(pdu_header->length);
    /* Do not check bounds with pdu_len yet, do it in the case blocks
     * below to make it possible to decode at least the beginning of
     * a truncated Error Report PDU or a truncated encapsulated PDU.
     */
     hexdump = FALSE;
 
     ND_PRINT((ndo, "%sRPKI-RTRv%u, %s PDU (%u), length: %u",
 	   indent_string(8),
 	   pdu_header->version,
 	   tok2str(rpki_rtr_pdu_values, "Unknown", pdu_type),
 	   pdu_type, pdu_len));
    if (pdu_len < sizeof(rpki_rtr_pdu) || pdu_len > len)
	goto invalid;
 
     switch (pdu_type) {
 
	/*
	 * The following PDUs share the message format.
	 */
     case RPKI_RTR_SERIAL_NOTIFY_PDU:
     case RPKI_RTR_SERIAL_QUERY_PDU:
     case RPKI_RTR_END_OF_DATA_PDU:
	if (pdu_len != sizeof(rpki_rtr_pdu) + 4)
	    goto invalid;
	ND_TCHECK2(*tptr, pdu_len);
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
	if (pdu_len != sizeof(rpki_rtr_pdu))
	    goto invalid;
	/* no additional boundary to check */
 
 	/*
 	 * Zero payload PDUs.
 	 */
 	break;
 
     case RPKI_RTR_CACHE_RESPONSE_PDU:
	if (pdu_len != sizeof(rpki_rtr_pdu))
	    goto invalid;
	/* no additional boundary to check */
 	ND_PRINT((ndo, "%sSession ID: 0x%04x",
 	       indent_string(indent+2),
 	       EXTRACT_16BITS(pdu_header->u.session_id)));
	break;

    case RPKI_RTR_IPV4_PREFIX_PDU:
 	{
 	    const rpki_rtr_pdu_ipv4_prefix *pdu;
 
	    if (pdu_len != sizeof(rpki_rtr_pdu) + 12)
		goto invalid;
	    ND_TCHECK2(*tptr, pdu_len);
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
 
	    if (pdu_len != sizeof(rpki_rtr_pdu) + 24)
		goto invalid;
	    ND_TCHECK2(*tptr, pdu_len);
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
 
	    tlen = sizeof(rpki_rtr_pdu);
	    /* Do not test for the "Length of Error Text" data element yet. */
	    if (pdu_len < tlen + 4)
		goto invalid;
	    ND_TCHECK2(*tptr, tlen + 4);
	    /* Safe up to and including the "Length of Encapsulated PDU"
	     * data element, more data elements may be present.
	     */
 	    pdu = (const rpki_rtr_pdu_error_report *)tptr;
 	    encapsulated_pdu_length = EXTRACT_32BITS(pdu->encapsulated_pdu_length);
	    tlen += 4;
 
 	    error_code = EXTRACT_16BITS(pdu->pdu_header.u.error_code);
 	    ND_PRINT((ndo, "%sError code: %s (%u), Encapsulated PDU length: %u",
 		   indent_string(indent+2),
 		   tok2str(rpki_rtr_error_codes, "Unknown", error_code),
 		   error_code, encapsulated_pdu_length));
 
	    if (encapsulated_pdu_length) {
		/* Section 5.10 of RFC 6810 says:
		 * "An Error Report PDU MUST NOT be sent for an Error Report PDU."
		 *
		 * However, as far as the protocol encoding goes Error Report PDUs can
		 * happen to be nested in each other, however many times, in which case
		 * the decoder should still print such semantically incorrect PDUs.
		 *
		 * That said, "the Erroneous PDU field MAY be truncated" (ibid), thus
		 * to keep things simple this implementation decodes only the two
		 * outermost layers of PDUs and makes bounds checks in the outer and
		 * the inner PDU independently.
		 */
		if (pdu_len < tlen + encapsulated_pdu_length)
		    goto invalid;
		if (! recurse) {
		    ND_TCHECK2(*tptr, tlen + encapsulated_pdu_length);
		}
		else {
		    ND_PRINT((ndo, "%s-----encapsulated PDU-----", indent_string(indent+4)));
		    rpki_rtr_pdu_print(ndo, tptr + tlen,
			encapsulated_pdu_length, 0, indent + 2);
		}
		tlen += encapsulated_pdu_length;
 	    }
 
	    if (pdu_len < tlen + 4)
		goto invalid;
	    ND_TCHECK2(*tptr, tlen + 4);
	    /* Safe up to and including the "Length of Error Text" data element,
	     * one more data element may be present.
	     */
 
 	    /*
 	     * Extract, trail-zero and print the Error message.
 	     */
	    text_length = EXTRACT_32BITS(tptr + tlen);
	    tlen += 4;

	    if (text_length) {
		if (pdu_len < tlen + text_length)
		    goto invalid;
		/* fn_printn() makes the bounds check */
 		ND_PRINT((ndo, "%sError text: ", indent_string(indent+2)));
		if (fn_printn(ndo, tptr + tlen, text_length, ndo->ndo_snapend))
 			goto trunc;
 	    }
 	}
 	break;
 
     default:
	ND_TCHECK2(*tptr, pdu_len);
 
 	/*
 	 * Unknown data, please hexdump.
	 */
	hexdump = TRUE;
    }

    /* do we also want to see a hex dump ? */
     if (ndo->ndo_vflag > 1 || (ndo->ndo_vflag && hexdump)) {
 	print_unknown_data(ndo,tptr,"\n\t  ", pdu_len);
     }
    return pdu_len;
 
invalid:
    ND_PRINT((ndo, "%s", istr));
    ND_TCHECK2(*tptr, len);
    return len;
 trunc:
    ND_PRINT((ndo, "\n\t%s", tstr));
    return len;
 }
