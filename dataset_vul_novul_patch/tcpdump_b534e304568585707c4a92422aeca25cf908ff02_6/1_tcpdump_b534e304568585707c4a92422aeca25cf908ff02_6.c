juniper_services_print(netdissect_options *ndo,
                       const struct pcap_pkthdr *h, register const u_char *p)
{
        struct juniper_l2info_t l2info;
        struct juniper_services_header {
            uint8_t svc_id;
            uint8_t flags_len;
            uint8_t svc_set_id[2];
            uint8_t dir_iif[4];
        };
        const struct juniper_services_header *sh;

        l2info.pictype = DLT_JUNIPER_SERVICES;
        if (juniper_parse_header(ndo, p, h, &l2info) == 0)
            return l2info.header_len;

         p+=l2info.header_len;
         sh = (const struct juniper_services_header *)p;
 
         if (ndo->ndo_eflag)
             ND_PRINT((ndo, "service-id %u flags 0x%02x service-set-id 0x%04x iif %u: ",
                    sh->svc_id,
                   sh->flags_len,
                   EXTRACT_16BITS(&sh->svc_set_id),
                   EXTRACT_24BITS(&sh->dir_iif[1])));

        /* no proto field - lets guess by first byte of IP header*/
         ip_heuristic_guess (ndo, p, l2info.length);
 
         return l2info.header_len;
 }
