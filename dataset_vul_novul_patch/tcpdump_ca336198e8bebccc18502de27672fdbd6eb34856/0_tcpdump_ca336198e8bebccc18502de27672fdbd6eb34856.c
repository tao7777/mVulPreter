pktap_if_print(netdissect_options *ndo,
               const struct pcap_pkthdr *h, const u_char *p)
{
	uint32_t dlt, hdrlen, rectype;
	u_int caplen = h->caplen;
 	u_int length = h->len;
 	if_printer printer;
 	const pktap_header_t *hdr;
	struct pcap_pkthdr nhdr;
 
 	if (caplen < sizeof(pktap_header_t) || length < sizeof(pktap_header_t)) {
 		ND_PRINT((ndo, "[|pktap]"));
		return (0);
	}
	hdr = (const pktap_header_t *)p;
	dlt = EXTRACT_LE_32BITS(&hdr->pkt_dlt);
	hdrlen = EXTRACT_LE_32BITS(&hdr->pkt_len);
	if (hdrlen < sizeof(pktap_header_t)) {
		/*
		 * Claimed header length < structure length.
		 * XXX - does this just mean some fields aren't
		 * being supplied, or is it truly an error (i.e.,
		 * is the length supplied so that the header can
		 * be expanded in the future)?
		 */
		ND_PRINT((ndo, "[|pktap]"));
		return (0);
	}
	if (caplen < hdrlen || length < hdrlen) {
		ND_PRINT((ndo, "[|pktap]"));
		return (hdrlen);
	}

	if (ndo->ndo_eflag)
		pktap_header_print(ndo, p, length);

	length -= hdrlen;
	caplen -= hdrlen;
	p += hdrlen;

	rectype = EXTRACT_LE_32BITS(&hdr->pkt_rectype);
	switch (rectype) {

	case PKT_REC_NONE:
		ND_PRINT((ndo, "no data"));
		break;
 
 	case PKT_REC_PACKET:
 		if ((printer = lookup_printer(dlt)) != NULL) {
			nhdr = *h;
			nhdr.caplen = caplen;
			nhdr.len = length;
			hdrlen += printer(ndo, &nhdr, p);
 		} else {
 			if (!ndo->ndo_eflag)
 				pktap_header_print(ndo, (const u_char *)hdr,
						length + hdrlen);

			if (!ndo->ndo_suppress_default_print)
				ND_DEFAULTPRINT(p, caplen);
		}
		break;
	}

	return (hdrlen);
}
