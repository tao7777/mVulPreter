 rpki_rtr_print(netdissect_options *ndo, register const u_char *pptr, register u_int len)
 {
     if (!ndo->ndo_vflag) {
 	ND_PRINT((ndo, ", RPKI-RTR"));
 	return;
     }
    while (len) {
	u_int pdu_len = rpki_rtr_pdu_print(ndo, pptr, len, 1, 8);
	len -= pdu_len;
	pptr += pdu_len;
     }
 }
