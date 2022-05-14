juniper_atm2_print(netdissect_options *ndo,
                   const struct pcap_pkthdr *h, register const u_char *p)
{
        int llc_hdrlen;

        struct juniper_l2info_t l2info;

        l2info.pictype = DLT_JUNIPER_ATM2;
        if (juniper_parse_header(ndo, p, h, &l2info) == 0)
            return l2info.header_len;

        p+=l2info.header_len;

        if (l2info.cookie[7] & ATM2_PKT_TYPE_MASK) { /* OAM cell ? */
            oam_print(ndo, p, l2info.length, ATM_OAM_NOHEC);
             return l2info.header_len;
         }
 
         if (EXTRACT_24BITS(p) == 0xfefe03 || /* NLPID encaps ? */
             EXTRACT_24BITS(p) == 0xaaaa03) { /* SNAP encaps ? */
 
            llc_hdrlen = llc_print(ndo, p, l2info.length, l2info.caplen, NULL, NULL);
            if (llc_hdrlen > 0)
                return l2info.header_len;
        }

        if (l2info.direction != JUNIPER_BPF_PKT_IN && /* ether-over-1483 encaps ? */
            (EXTRACT_32BITS(l2info.cookie) & ATM2_GAP_COUNT_MASK)) {
            ether_print(ndo, p, l2info.length, l2info.caplen, NULL, NULL);
            return l2info.header_len;
        }

        if (p[0] == 0x03) { /* Cisco style NLPID encaps ? */
            isoclns_print(ndo, p + 1, l2info.length - 1);
            /* FIXME check if frame was recognized */
            return l2info.header_len;
        }

        if(juniper_ppp_heuristic_guess(ndo, p, l2info.length) != 0) /* PPPoA vcmux encaps ? */
            return l2info.header_len;

        if (ip_heuristic_guess(ndo, p, l2info.length) != 0) /* last try - vcmux encaps ? */
             return l2info.header_len;
 
 	return l2info.header_len;
 }
