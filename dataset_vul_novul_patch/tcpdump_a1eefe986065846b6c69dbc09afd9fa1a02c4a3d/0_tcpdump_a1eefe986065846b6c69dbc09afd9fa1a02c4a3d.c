 chdlc_if_print(netdissect_options *ndo, const struct pcap_pkthdr *h, register const u_char *p)
 {
	return chdlc_print(ndo, p, h->len);
 }
