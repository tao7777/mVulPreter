juniper_ggsn_print(netdissect_options *ndo,
                   const struct pcap_pkthdr *h, register const u_char *p)
{
        struct juniper_l2info_t l2info;
        struct juniper_ggsn_header {
            uint8_t svc_id;
            uint8_t flags_len;
            uint8_t proto;
            uint8_t flags;
            uint8_t vlan_id[2];
            uint8_t res[2];
        };
        const struct juniper_ggsn_header *gh;

        l2info.pictype = DLT_JUNIPER_GGSN;
        if (juniper_parse_header(ndo, p, h, &l2info) == 0)
            return l2info.header_len;

         p+=l2info.header_len;
         gh = (struct juniper_ggsn_header *)&l2info.cookie;
 
        ND_TCHECK(*gh);
         if (ndo->ndo_eflag) {
             ND_PRINT((ndo, "proto %s (%u), vlan %u: ",
                    tok2str(juniper_protocol_values,"Unknown",gh->proto),
                   gh->proto,
                   EXTRACT_16BITS(&gh->vlan_id[0])));
        }

        switch (gh->proto) {
        case JUNIPER_PROTO_IPV4:
            ip_print(ndo, p, l2info.length);
            break;
        case JUNIPER_PROTO_IPV6:
            ip6_print(ndo, p, l2info.length);
            break;
        default:
            if (!ndo->ndo_eflag)
                ND_PRINT((ndo, "unknown GGSN proto (%u)", gh->proto));
         }
 
         return l2info.header_len;

trunc:
	ND_PRINT((ndo, "[|juniper_services]"));
	return l2info.header_len;
 }
