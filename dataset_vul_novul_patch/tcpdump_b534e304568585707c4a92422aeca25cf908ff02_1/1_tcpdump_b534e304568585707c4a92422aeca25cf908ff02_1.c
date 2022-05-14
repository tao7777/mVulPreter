juniper_es_print(netdissect_options *ndo,
                 const struct pcap_pkthdr *h, register const u_char *p)
{
        struct juniper_l2info_t l2info;
        struct juniper_ipsec_header {
            uint8_t sa_index[2];
            uint8_t ttl;
            uint8_t type;
            uint8_t spi[4];
            uint8_t src_ip[4];
            uint8_t dst_ip[4];
        };
        u_int rewrite_len,es_type_bundle;
        const struct juniper_ipsec_header *ih;

        l2info.pictype = DLT_JUNIPER_ES;
        if (juniper_parse_header(ndo, p, h, &l2info) == 0)
            return l2info.header_len;

         p+=l2info.header_len;
         ih = (const struct juniper_ipsec_header *)p;
 
         switch (ih->type) {
         case JUNIPER_IPSEC_O_ESP_ENCRYPT_ESP_AUTHEN_TYPE:
         case JUNIPER_IPSEC_O_ESP_ENCRYPT_AH_AUTHEN_TYPE:
            rewrite_len = 0;
            es_type_bundle = 1;
            break;
        case JUNIPER_IPSEC_O_ESP_AUTHENTICATION_TYPE:
        case JUNIPER_IPSEC_O_AH_AUTHENTICATION_TYPE:
        case JUNIPER_IPSEC_O_ESP_ENCRYPTION_TYPE:
            rewrite_len = 16;
            es_type_bundle = 0;
            break;
        default:
            ND_PRINT((ndo, "ES Invalid type %u, length %u",
                   ih->type,
                   l2info.length));
            return l2info.header_len;
        }

        l2info.length-=rewrite_len;
        p+=rewrite_len;

        if (ndo->ndo_eflag) {
            if (!es_type_bundle) {
                ND_PRINT((ndo, "ES SA, index %u, ttl %u type %s (%u), spi %u, Tunnel %s > %s, length %u\n",
                       EXTRACT_16BITS(&ih->sa_index),
                       ih->ttl,
                       tok2str(juniper_ipsec_type_values,"Unknown",ih->type),
                       ih->type,
                       EXTRACT_32BITS(&ih->spi),
                       ipaddr_string(ndo, &ih->src_ip),
                       ipaddr_string(ndo, &ih->dst_ip),
                       l2info.length));
            } else {
                ND_PRINT((ndo, "ES SA, index %u, ttl %u type %s (%u), length %u\n",
                       EXTRACT_16BITS(&ih->sa_index),
                       ih->ttl,
                       tok2str(juniper_ipsec_type_values,"Unknown",ih->type),
                       ih->type,
                       l2info.length));
            }
        }
 
         ip_print(ndo, p, l2info.length);
         return l2info.header_len;
 }
