juniper_monitor_print(netdissect_options *ndo,
                      const struct pcap_pkthdr *h, register const u_char *p)
{
        struct juniper_l2info_t l2info;
        struct juniper_monitor_header {
            uint8_t pkt_type;
            uint8_t padding;
            uint8_t iif[2];
            uint8_t service_id[4];
        };
        const struct juniper_monitor_header *mh;

        l2info.pictype = DLT_JUNIPER_MONITOR;
        if (juniper_parse_header(ndo, p, h, &l2info) == 0)
            return l2info.header_len;

         p+=l2info.header_len;
         mh = (const struct juniper_monitor_header *)p;
 
         if (ndo->ndo_eflag)
             ND_PRINT((ndo, "service-id %u, iif %u, pkt-type %u: ",
                    EXTRACT_32BITS(&mh->service_id),
                   EXTRACT_16BITS(&mh->iif),
                   mh->pkt_type));

        /* no proto field - lets guess by first byte of IP header*/
         ip_heuristic_guess (ndo, p, l2info.length);
 
         return l2info.header_len;
 }
