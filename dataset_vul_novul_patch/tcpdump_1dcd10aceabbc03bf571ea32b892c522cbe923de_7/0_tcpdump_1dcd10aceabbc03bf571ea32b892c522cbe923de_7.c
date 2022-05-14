juniper_mfr_print(netdissect_options *ndo,
                  const struct pcap_pkthdr *h, register const u_char *p)
{
        struct juniper_l2info_t l2info;

        memset(&l2info, 0, sizeof(l2info));
        l2info.pictype = DLT_JUNIPER_MFR;
        if (juniper_parse_header(ndo, p, h, &l2info) == 0)
            return l2info.header_len;

        p+=l2info.header_len;

        /* child-link ? */
        if (l2info.cookie_len == 0) {
            mfr_print(ndo, p, l2info.length);
            return l2info.header_len;
        }

        /* first try the LSQ protos */
        if (l2info.cookie_len == AS_PIC_COOKIE_LEN) {
            switch(l2info.proto) {
            case JUNIPER_LSQ_L3_PROTO_IPV4:
                ip_print(ndo, p, l2info.length);
                return l2info.header_len;
            case JUNIPER_LSQ_L3_PROTO_IPV6:
                ip6_print(ndo, p,l2info.length);
                return l2info.header_len;
            case JUNIPER_LSQ_L3_PROTO_MPLS:
                 mpls_print(ndo, p, l2info.length);
                 return l2info.header_len;
             case JUNIPER_LSQ_L3_PROTO_ISO:
                isoclns_print(ndo, p, l2info.length);
                 return l2info.header_len;
             default:
                 break;
            }
            return l2info.header_len;
        }

        /* suppress Bundle-ID if frame was captured on a child-link */
        if (ndo->ndo_eflag && EXTRACT_32BITS(l2info.cookie) != 1)
             ND_PRINT((ndo, "Bundle-ID %u, ", l2info.bundle));
         switch (l2info.proto) {
         case (LLCSAP_ISONS<<8 | LLCSAP_ISONS):
            isoclns_print(ndo, p + 1, l2info.length - 1);
             break;
         case (LLC_UI<<8 | NLPID_Q933):
         case (LLC_UI<<8 | NLPID_IP):
         case (LLC_UI<<8 | NLPID_IP6):
             /* pass IP{4,6} to the OSI layer for proper link-layer printing */
            isoclns_print(ndo, p - 1, l2info.length + 1);
             break;
         default:
             ND_PRINT((ndo, "unknown protocol 0x%04x, length %u", l2info.proto, l2info.length));
        }

        return l2info.header_len;
}
