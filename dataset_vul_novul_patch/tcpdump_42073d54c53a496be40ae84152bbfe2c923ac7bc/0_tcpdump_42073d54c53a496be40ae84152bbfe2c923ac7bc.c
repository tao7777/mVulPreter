juniper_parse_header(netdissect_options *ndo,
                     const u_char *p, const struct pcap_pkthdr *h, struct juniper_l2info_t *l2info)
{
    const struct juniper_cookie_table_t *lp = juniper_cookie_table;
    u_int idx, jnx_ext_len, jnx_header_len = 0;
    uint8_t tlv_type,tlv_len;
    uint32_t control_word;
    int tlv_value;
    const u_char *tptr;


    l2info->header_len = 0;
    l2info->cookie_len = 0;
    l2info->proto = 0;


    l2info->length = h->len;
    l2info->caplen = h->caplen;
    ND_TCHECK2(p[0], 4);
    l2info->flags = p[3];
    l2info->direction = p[3]&JUNIPER_BPF_PKT_IN;

    if (EXTRACT_24BITS(p) != JUNIPER_MGC_NUMBER) { /* magic number found ? */
        ND_PRINT((ndo, "no magic-number found!"));
        return 0;
    }

    if (ndo->ndo_eflag) /* print direction */
        ND_PRINT((ndo, "%3s ", tok2str(juniper_direction_values, "---", l2info->direction)));

    /* magic number + flags */
    jnx_header_len = 4;

    if (ndo->ndo_vflag > 1)
        ND_PRINT((ndo, "\n\tJuniper PCAP Flags [%s]",
               bittok2str(jnx_flag_values, "none", l2info->flags)));

    /* extensions present ?  - calculate how much bytes to skip */
    if ((l2info->flags & JUNIPER_BPF_EXT ) == JUNIPER_BPF_EXT ) {

        tptr = p+jnx_header_len;

        /* ok to read extension length ? */
        ND_TCHECK2(tptr[0], 2);
        jnx_ext_len = EXTRACT_16BITS(tptr);
        jnx_header_len += 2;
        tptr +=2;

        /* nail up the total length -
         * just in case something goes wrong
         * with TLV parsing */
        jnx_header_len += jnx_ext_len;

        if (ndo->ndo_vflag > 1)
            ND_PRINT((ndo, ", PCAP Extension(s) total length %u", jnx_ext_len));

        ND_TCHECK2(tptr[0], jnx_ext_len);
        while (jnx_ext_len > JUNIPER_EXT_TLV_OVERHEAD) {
            tlv_type = *(tptr++);
            tlv_len = *(tptr++);
            tlv_value = 0;

            /* sanity checks */
            if (tlv_type == 0 || tlv_len == 0)
                break;
            if (tlv_len+JUNIPER_EXT_TLV_OVERHEAD > jnx_ext_len)
                goto trunc;

            if (ndo->ndo_vflag > 1)
                ND_PRINT((ndo, "\n\t  %s Extension TLV #%u, length %u, value ",
                       tok2str(jnx_ext_tlv_values,"Unknown",tlv_type),
                       tlv_type,
                       tlv_len));

            tlv_value = juniper_read_tlv_value(tptr, tlv_type, tlv_len);
            switch (tlv_type) {
            case JUNIPER_EXT_TLV_IFD_NAME:
                /* FIXME */
                break;
            case JUNIPER_EXT_TLV_IFD_MEDIATYPE:
            case JUNIPER_EXT_TLV_TTP_IFD_MEDIATYPE:
                if (tlv_value != -1) {
                    if (ndo->ndo_vflag > 1)
                        ND_PRINT((ndo, "%s (%u)",
                               tok2str(juniper_ifmt_values, "Unknown", tlv_value),
                               tlv_value));
                }
                break;
            case JUNIPER_EXT_TLV_IFL_ENCAPS:
            case JUNIPER_EXT_TLV_TTP_IFL_ENCAPS:
                if (tlv_value != -1) {
                    if (ndo->ndo_vflag > 1)
                        ND_PRINT((ndo, "%s (%u)",
                               tok2str(juniper_ifle_values, "Unknown", tlv_value),
                               tlv_value));
                }
                break;
            case JUNIPER_EXT_TLV_IFL_IDX: /* fall through */
            case JUNIPER_EXT_TLV_IFL_UNIT:
            case JUNIPER_EXT_TLV_IFD_IDX:
            default:
                if (tlv_value != -1) {
                    if (ndo->ndo_vflag > 1)
                        ND_PRINT((ndo, "%u", tlv_value));
                }
                break;
            }

            tptr+=tlv_len;
            jnx_ext_len -= tlv_len+JUNIPER_EXT_TLV_OVERHEAD;
        }

        if (ndo->ndo_vflag > 1)
            ND_PRINT((ndo, "\n\t-----original packet-----\n\t"));
    }

    if ((l2info->flags & JUNIPER_BPF_NO_L2 ) == JUNIPER_BPF_NO_L2 ) {
        if (ndo->ndo_eflag)
            ND_PRINT((ndo, "no-L2-hdr, "));

        /* there is no link-layer present -
         * perform the v4/v6 heuristics
         * to figure out what it is
         */
        ND_TCHECK2(p[jnx_header_len + 4], 1);
        if (ip_heuristic_guess(ndo, p + jnx_header_len + 4,
                               l2info->length - (jnx_header_len + 4)) == 0)
            ND_PRINT((ndo, "no IP-hdr found!"));

        l2info->header_len=jnx_header_len+4;
        return 0; /* stop parsing the output further */

    }
    l2info->header_len = jnx_header_len;
    p+=l2info->header_len;
    l2info->length -= l2info->header_len;
    l2info->caplen -= l2info->header_len;

    /* search through the cookie table and copy values matching for our PIC type */
    ND_TCHECK(p[0]);
    while (lp->s != NULL) {
        if (lp->pictype == l2info->pictype) {

            l2info->cookie_len += lp->cookie_len;

            switch (p[0]) {
            case LS_COOKIE_ID:
                l2info->cookie_type = LS_COOKIE_ID;
                l2info->cookie_len += 2;
                break;
            case AS_COOKIE_ID:
                l2info->cookie_type = AS_COOKIE_ID;
                l2info->cookie_len = 8;
                break;

            default:
                l2info->bundle = l2info->cookie[0];
                break;
            }


#ifdef DLT_JUNIPER_MFR
            /* MFR child links don't carry cookies */
            if (l2info->pictype == DLT_JUNIPER_MFR &&
                (p[0] & MFR_BE_MASK) == MFR_BE_MASK) {
                l2info->cookie_len = 0;
            }
#endif

            l2info->header_len += l2info->cookie_len;
            l2info->length -= l2info->cookie_len;
            l2info->caplen -= l2info->cookie_len;

            if (ndo->ndo_eflag)
                ND_PRINT((ndo, "%s-PIC, cookie-len %u",
                       lp->s,
                       l2info->cookie_len));

            if (l2info->cookie_len > 0) {
                ND_TCHECK2(p[0], l2info->cookie_len);
                if (ndo->ndo_eflag)
                    ND_PRINT((ndo, ", cookie 0x"));
                for (idx = 0; idx < l2info->cookie_len; idx++) {
                    l2info->cookie[idx] = p[idx]; /* copy cookie data */
                    if (ndo->ndo_eflag) ND_PRINT((ndo, "%02x", p[idx]));
                }
            }

             if (ndo->ndo_eflag) ND_PRINT((ndo, ": ")); /* print demarc b/w L2/L3*/
 
 
            ND_TCHECK_16BITS(p+l2info->cookie_len);
             l2info->proto = EXTRACT_16BITS(p+l2info->cookie_len);
             break;
         }
        ++lp;
    }
    p+=l2info->cookie_len;

    /* DLT_ specific parsing */
    switch(l2info->pictype) {
#ifdef DLT_JUNIPER_MLPPP
    case DLT_JUNIPER_MLPPP:
        switch (l2info->cookie_type) {
        case LS_COOKIE_ID:
            l2info->bundle = l2info->cookie[1];
            break;
        case AS_COOKIE_ID:
            l2info->bundle = (EXTRACT_16BITS(&l2info->cookie[6])>>3)&0xfff;
            l2info->proto = (l2info->cookie[5])&JUNIPER_LSQ_L3_PROTO_MASK;
            break;
        default:
            l2info->bundle = l2info->cookie[0];
            break;
        }
        break;
#endif
#ifdef DLT_JUNIPER_MLFR
    case DLT_JUNIPER_MLFR:
        switch (l2info->cookie_type) {
        case LS_COOKIE_ID:
            ND_TCHECK2(p[0], 2);
            l2info->bundle = l2info->cookie[1];
            l2info->proto = EXTRACT_16BITS(p);
            l2info->header_len += 2;
            l2info->length -= 2;
            l2info->caplen -= 2;
            break;
        case AS_COOKIE_ID:
            l2info->bundle = (EXTRACT_16BITS(&l2info->cookie[6])>>3)&0xfff;
            l2info->proto = (l2info->cookie[5])&JUNIPER_LSQ_L3_PROTO_MASK;
            break;
        default:
            l2info->bundle = l2info->cookie[0];
            l2info->header_len += 2;
            l2info->length -= 2;
            l2info->caplen -= 2;
            break;
        }
        break;
#endif
#ifdef DLT_JUNIPER_MFR
    case DLT_JUNIPER_MFR:
        switch (l2info->cookie_type) {
        case LS_COOKIE_ID:
            ND_TCHECK2(p[0], 2);
            l2info->bundle = l2info->cookie[1];
            l2info->proto = EXTRACT_16BITS(p);
            l2info->header_len += 2;
            l2info->length -= 2;
            l2info->caplen -= 2;
            break;
        case AS_COOKIE_ID:
            l2info->bundle = (EXTRACT_16BITS(&l2info->cookie[6])>>3)&0xfff;
            l2info->proto = (l2info->cookie[5])&JUNIPER_LSQ_L3_PROTO_MASK;
            break;
        default:
            l2info->bundle = l2info->cookie[0];
            break;
        }
        break;
#endif
#ifdef DLT_JUNIPER_ATM2
    case DLT_JUNIPER_ATM2:
        ND_TCHECK2(p[0], 4);
        /* ATM cell relay control word present ? */
        if (l2info->cookie[7] & ATM2_PKT_TYPE_MASK) {
            control_word = EXTRACT_32BITS(p);
            /* some control word heuristics */
            switch(control_word) {
            case 0: /* zero control word */
            case 0x08000000: /* < JUNOS 7.4 control-word */
            case 0x08380000: /* cntl word plus cell length (56) >= JUNOS 7.4*/
                l2info->header_len += 4;
                break;
            default:
                break;
            }

            if (ndo->ndo_eflag)
                ND_PRINT((ndo, "control-word 0x%08x ", control_word));
        }
        break;
#endif
#ifdef DLT_JUNIPER_GGSN
    case DLT_JUNIPER_GGSN:
        break;
#endif
#ifdef DLT_JUNIPER_ATM1
    case DLT_JUNIPER_ATM1:
        break;
#endif
#ifdef DLT_JUNIPER_PPP
    case DLT_JUNIPER_PPP:
        break;
#endif
#ifdef DLT_JUNIPER_CHDLC
    case DLT_JUNIPER_CHDLC:
        break;
#endif
#ifdef DLT_JUNIPER_ETHER
    case DLT_JUNIPER_ETHER:
        break;
#endif
#ifdef DLT_JUNIPER_FRELAY
    case DLT_JUNIPER_FRELAY:
        break;
#endif

    default:
        ND_PRINT((ndo, "Unknown Juniper DLT_ type %u: ", l2info->pictype));
        break;
    }

    if (ndo->ndo_eflag > 1)
        ND_PRINT((ndo, "hlen %u, proto 0x%04x, ", l2info->header_len, l2info->proto));

    return 1; /* everything went ok so far. continue parsing */
 trunc:
    ND_PRINT((ndo, "[|juniper_hdr], length %u", h->len));
    return 0;
}
