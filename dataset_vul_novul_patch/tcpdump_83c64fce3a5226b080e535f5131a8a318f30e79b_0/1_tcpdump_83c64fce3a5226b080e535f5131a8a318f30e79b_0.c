 rpki_rtr_print(netdissect_options *ndo, register const u_char *pptr, register u_int len)
 {
    u_int tlen, pdu_type, pdu_len;
    const u_char *tptr;
    const rpki_rtr_pdu *pdu_header;
    tptr = pptr;
    tlen = len;
     if (!ndo->ndo_vflag) {
 	ND_PRINT((ndo, ", RPKI-RTR"));
 	return;
     }
    while (tlen >= sizeof(rpki_rtr_pdu)) {
        ND_TCHECK2(*tptr, sizeof(rpki_rtr_pdu));
	pdu_header = (const rpki_rtr_pdu *)tptr;
        pdu_type = pdu_header->pdu_type;
        pdu_len = EXTRACT_32BITS(pdu_header->length);
        ND_TCHECK2(*tptr, pdu_len);
        /* infinite loop check */
        if (!pdu_type || !pdu_len) {
            break;
        }
        if (tlen < pdu_len) {
            goto trunc;
        }
	/*
	 * Print the PDU.
	 */
	if (rpki_rtr_pdu_print(ndo, tptr, 8))
		goto trunc;
        tlen -= pdu_len;
        tptr += pdu_len;
     }
    return;
trunc:
    ND_PRINT((ndo, "\n\t%s", tstr));
 }
