juniper_mlfr_print(netdissect_options *ndo,
                   const struct pcap_pkthdr *h, register const u_char *p)
{
        struct juniper_l2info_t l2info;

        l2info.pictype = DLT_JUNIPER_MLFR;
        if (juniper_parse_header(ndo, p, h, &l2info) == 0)
            return l2info.header_len;

        p+=l2info.header_len;

        /* suppress Bundle-ID if frame was captured on a child-link */
        if (ndo->ndo_eflag && EXTRACT_32BITS(l2info.cookie) != 1)
            ND_PRINT((ndo, "Bundle-ID %u, ", l2info.bundle));
         switch (l2info.proto) {
         case (LLC_UI):
         case (LLC_UI<<8):
            isoclns_print(ndo, p, l2info.length, l2info.caplen);
             break;
         case (LLC_UI<<8 | NLPID_Q933):
         case (LLC_UI<<8 | NLPID_IP):
         case (LLC_UI<<8 | NLPID_IP6):
             /* pass IP{4,6} to the OSI layer for proper link-layer printing */
            isoclns_print(ndo, p - 1, l2info.length + 1, l2info.caplen + 1);
             break;
         default:
             ND_PRINT((ndo, "unknown protocol 0x%04x, length %u", l2info.proto, l2info.length));
        }

        return l2info.header_len;
}
