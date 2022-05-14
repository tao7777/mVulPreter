juniper_mlppp_print(netdissect_options *ndo,
                    const struct pcap_pkthdr *h, register const u_char *p)
{
        struct juniper_l2info_t l2info;

        l2info.pictype = DLT_JUNIPER_MLPPP;
        if (juniper_parse_header(ndo, p, h, &l2info) == 0)
            return l2info.header_len;

        /* suppress Bundle-ID if frame was captured on a child-link
         * best indicator if the cookie looks like a proto */
        if (ndo->ndo_eflag &&
            EXTRACT_16BITS(&l2info.cookie) != PPP_OSI &&
            EXTRACT_16BITS(&l2info.cookie) !=  (PPP_ADDRESS << 8 | PPP_CONTROL))
            ND_PRINT((ndo, "Bundle-ID %u: ", l2info.bundle));

        p+=l2info.header_len;

        /* first try the LSQ protos */
        switch(l2info.proto) {
        case JUNIPER_LSQ_L3_PROTO_IPV4:
            /* IP traffic going to the RE would not have a cookie
             * -> this must be incoming IS-IS over PPP
             */
            if (l2info.cookie[4] == (JUNIPER_LSQ_COOKIE_RE|JUNIPER_LSQ_COOKIE_DIR))
                ppp_print(ndo, p, l2info.length);
            else
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

        /* zero length cookie ? */
        switch (EXTRACT_16BITS(&l2info.cookie)) {
        case PPP_OSI:
            ppp_print(ndo, p - 2, l2info.length + 2);
            break;
        case (PPP_ADDRESS << 8 | PPP_CONTROL): /* fall through */
        default:
            ppp_print(ndo, p, l2info.length);
            break;
        }

        return l2info.header_len;
}
