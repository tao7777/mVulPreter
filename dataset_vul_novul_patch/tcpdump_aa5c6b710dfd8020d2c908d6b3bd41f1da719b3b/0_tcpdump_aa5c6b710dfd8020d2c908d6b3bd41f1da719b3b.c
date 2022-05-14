ldp_pdu_print(netdissect_options *ndo,
              register const u_char *pptr)
{
    const struct ldp_common_header *ldp_com_header;
    const struct ldp_msg_header *ldp_msg_header;
    const u_char *tptr,*msg_tptr;
    u_short tlen;
    u_short pdu_len,msg_len,msg_type,msg_tlen;
    int hexdump,processed;

    ldp_com_header = (const struct ldp_common_header *)pptr;
    ND_TCHECK(*ldp_com_header);

    /*
     * Sanity checking of the header.
     */
    if (EXTRACT_16BITS(&ldp_com_header->version) != LDP_VERSION) {
	ND_PRINT((ndo, "%sLDP version %u packet not supported",
               (ndo->ndo_vflag < 1) ? "" : "\n\t",
               EXTRACT_16BITS(&ldp_com_header->version)));
	return 0;
    }

    pdu_len = EXTRACT_16BITS(&ldp_com_header->pdu_length);
    if (pdu_len < sizeof(const struct ldp_common_header)-4) {
        /* length too short */
        ND_PRINT((ndo, "%sLDP, pdu-length: %u (too short, < %u)",
               (ndo->ndo_vflag < 1) ? "" : "\n\t",
               pdu_len,
               (u_int)(sizeof(const struct ldp_common_header)-4)));
        return 0;
    }

    /* print the LSR-ID, label-space & length */
    ND_PRINT((ndo, "%sLDP, Label-Space-ID: %s:%u, pdu-length: %u",
           (ndo->ndo_vflag < 1) ? "" : "\n\t",
           ipaddr_string(ndo, &ldp_com_header->lsr_id),
           EXTRACT_16BITS(&ldp_com_header->label_space),
           pdu_len));

    /* bail out if non-verbose */
    if (ndo->ndo_vflag < 1)
        return 0;

    /* ok they seem to want to know everything - lets fully decode it */
    tptr = pptr + sizeof(const struct ldp_common_header);
    tlen = pdu_len - (sizeof(const struct ldp_common_header)-4);	/* Type & Length fields not included */

    while(tlen>0) {
        /* did we capture enough for fully decoding the msg header ? */
        ND_TCHECK2(*tptr, sizeof(struct ldp_msg_header));

        ldp_msg_header = (const struct ldp_msg_header *)tptr;
        msg_len=EXTRACT_16BITS(ldp_msg_header->length);
        msg_type=LDP_MASK_MSG_TYPE(EXTRACT_16BITS(ldp_msg_header->type));

        if (msg_len < sizeof(struct ldp_msg_header)-4) {
            /* length too short */
            /* FIXME vendor private / experimental check */
            ND_PRINT((ndo, "\n\t  %s Message (0x%04x), length: %u (too short, < %u)",
                   tok2str(ldp_msg_values,
                           "Unknown",
                           msg_type),
                   msg_type,
                   msg_len,
                   (u_int)(sizeof(struct ldp_msg_header)-4)));
            return 0;
        }

        /* FIXME vendor private / experimental check */
        ND_PRINT((ndo, "\n\t  %s Message (0x%04x), length: %u, Message ID: 0x%08x, Flags: [%s if unknown]",
               tok2str(ldp_msg_values,
                       "Unknown",
                       msg_type),
               msg_type,
               msg_len,
               EXTRACT_32BITS(&ldp_msg_header->id),
               LDP_MASK_U_BIT(EXTRACT_16BITS(&ldp_msg_header->type)) ? "continue processing" : "ignore"));

        msg_tptr=tptr+sizeof(struct ldp_msg_header);
        msg_tlen=msg_len-(sizeof(struct ldp_msg_header)-4); /* Type & Length fields not included */

        /* did we capture enough for fully decoding the message ? */
        ND_TCHECK2(*tptr, msg_len);
        hexdump=FALSE;

        switch(msg_type) {

        case LDP_MSG_NOTIF:
        case LDP_MSG_HELLO:
        case LDP_MSG_INIT:
        case LDP_MSG_KEEPALIVE:
        case LDP_MSG_ADDRESS:
        case LDP_MSG_LABEL_MAPPING:
        case LDP_MSG_ADDRESS_WITHDRAW:
        case LDP_MSG_LABEL_WITHDRAW:
            while(msg_tlen >= 4) {
                processed = ldp_tlv_print(ndo, msg_tptr, msg_tlen);
                if (processed == 0)
                    break;
                msg_tlen-=processed;
                msg_tptr+=processed;
            }
            break;

        /*
         *  FIXME those are the defined messages that lack a decoder
         *  you are welcome to contribute code ;-)
         */

        case LDP_MSG_LABEL_REQUEST:
        case LDP_MSG_LABEL_RELEASE:
        case LDP_MSG_LABEL_ABORT_REQUEST:

        default:
            if (ndo->ndo_vflag <= 1)
                print_unknown_data(ndo, msg_tptr, "\n\t  ", msg_tlen);
            break;
        }
        /* do we want to see an additionally hexdump ? */
        if (ndo->ndo_vflag > 1 || hexdump==TRUE)
            print_unknown_data(ndo, tptr+sizeof(struct ldp_msg_header), "\n\t  ",
                               msg_len);

        tptr += msg_len+4;
        tlen -= msg_len+4;
     }
     return pdu_len+4;
 trunc:
    ND_PRINT((ndo, "%s", tstr));
     return 0;
 }
