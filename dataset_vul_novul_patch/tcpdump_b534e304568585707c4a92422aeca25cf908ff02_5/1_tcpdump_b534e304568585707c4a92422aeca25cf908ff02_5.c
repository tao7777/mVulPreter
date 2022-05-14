juniper_pppoe_atm_print(netdissect_options *ndo,
                        const struct pcap_pkthdr *h, register const u_char *p)
{
        struct juniper_l2info_t l2info;
	uint16_t extracted_ethertype;

        l2info.pictype = DLT_JUNIPER_PPPOE_ATM;
        if (juniper_parse_header(ndo, p, h, &l2info) == 0)
            return l2info.header_len;
 
         p+=l2info.header_len;
 
         extracted_ethertype = EXTRACT_16BITS(p);
         /* this DLT contains nothing but raw PPPoE frames,
          * prepended with a type field*/
        if (ethertype_print(ndo, extracted_ethertype,
                              p+ETHERTYPE_LEN,
                              l2info.length-ETHERTYPE_LEN,
                              l2info.caplen-ETHERTYPE_LEN,
                              NULL, NULL) == 0)
            /* ether_type not known, probably it wasn't one */
             ND_PRINT((ndo, "unknown ethertype 0x%04x", extracted_ethertype));
 
         return l2info.header_len;
 }
