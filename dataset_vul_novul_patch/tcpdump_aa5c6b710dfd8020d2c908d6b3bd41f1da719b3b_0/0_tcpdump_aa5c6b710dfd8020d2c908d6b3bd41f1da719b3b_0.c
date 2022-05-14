ldp_tlv_print(netdissect_options *ndo,
              register const u_char *tptr,
              u_short msg_tlen)
{
    struct ldp_tlv_header {
        uint8_t type[2];
        uint8_t length[2];
    };

    const struct ldp_tlv_header *ldp_tlv_header;
    u_short tlv_type,tlv_len,tlv_tlen,af,ft_flags;
    u_char fec_type;
    u_int ui,vc_info_len, vc_info_tlv_type, vc_info_tlv_len,idx;
    char buf[100];
    int i;

    ldp_tlv_header = (const struct ldp_tlv_header *)tptr;
    ND_TCHECK(*ldp_tlv_header);
    tlv_len=EXTRACT_16BITS(ldp_tlv_header->length);
    if (tlv_len + 4 > msg_tlen) {
        ND_PRINT((ndo, "\n\t\t TLV contents go past end of message"));
        return 0;
    }
    tlv_tlen=tlv_len;
    tlv_type=LDP_MASK_TLV_TYPE(EXTRACT_16BITS(ldp_tlv_header->type));

    /* FIXME vendor private / experimental check */
    ND_PRINT((ndo, "\n\t    %s TLV (0x%04x), length: %u, Flags: [%s and %s forward if unknown]",
           tok2str(ldp_tlv_values,
                   "Unknown",
                   tlv_type),
           tlv_type,
           tlv_len,
           LDP_MASK_U_BIT(EXTRACT_16BITS(&ldp_tlv_header->type)) ? "continue processing" : "ignore",
           LDP_MASK_F_BIT(EXTRACT_16BITS(&ldp_tlv_header->type)) ? "do" : "don't"));

    tptr+=sizeof(struct ldp_tlv_header);

    switch(tlv_type) {

    case LDP_TLV_COMMON_HELLO:
        TLV_TCHECK(4);
        ND_PRINT((ndo, "\n\t      Hold Time: %us, Flags: [%s Hello%s]",
               EXTRACT_16BITS(tptr),
               (EXTRACT_16BITS(tptr+2)&0x8000) ? "Targeted" : "Link",
               (EXTRACT_16BITS(tptr+2)&0x4000) ? ", Request for targeted Hellos" : ""));
        break;

    case LDP_TLV_IPV4_TRANSPORT_ADDR:
        TLV_TCHECK(4);
        ND_PRINT((ndo, "\n\t      IPv4 Transport Address: %s", ipaddr_string(ndo, tptr)));
        break;
    case LDP_TLV_IPV6_TRANSPORT_ADDR:
        TLV_TCHECK(16);
        ND_PRINT((ndo, "\n\t      IPv6 Transport Address: %s", ip6addr_string(ndo, tptr)));
        break;
    case LDP_TLV_CONFIG_SEQ_NUMBER:
        TLV_TCHECK(4);
        ND_PRINT((ndo, "\n\t      Sequence Number: %u", EXTRACT_32BITS(tptr)));
        break;

    case LDP_TLV_ADDRESS_LIST:
        TLV_TCHECK(LDP_TLV_ADDRESS_LIST_AFNUM_LEN);
	af = EXTRACT_16BITS(tptr);
	tptr+=LDP_TLV_ADDRESS_LIST_AFNUM_LEN;
        tlv_tlen -= LDP_TLV_ADDRESS_LIST_AFNUM_LEN;
	ND_PRINT((ndo, "\n\t      Address Family: %s, addresses",
               tok2str(af_values, "Unknown (%u)", af)));
        switch (af) {
        case AFNUM_INET:
	    while(tlv_tlen >= sizeof(struct in_addr)) {
		ND_TCHECK2(*tptr, sizeof(struct in_addr));
		ND_PRINT((ndo, " %s", ipaddr_string(ndo, tptr)));
		tlv_tlen-=sizeof(struct in_addr);
		tptr+=sizeof(struct in_addr);
	    }
            break;
        case AFNUM_INET6:
	    while(tlv_tlen >= sizeof(struct in6_addr)) {
		ND_TCHECK2(*tptr, sizeof(struct in6_addr));
		ND_PRINT((ndo, " %s", ip6addr_string(ndo, tptr)));
		tlv_tlen-=sizeof(struct in6_addr);
		tptr+=sizeof(struct in6_addr);
	    }
            break;
        default:
            /* unknown AF */
            break;
        }
	break;

    case LDP_TLV_COMMON_SESSION:
	TLV_TCHECK(8);
	ND_PRINT((ndo, "\n\t      Version: %u, Keepalive: %us, Flags: [Downstream %s, Loop Detection %s]",
	       EXTRACT_16BITS(tptr), EXTRACT_16BITS(tptr+2),
	       (EXTRACT_16BITS(tptr+6)&0x8000) ? "On Demand" : "Unsolicited",
	       (EXTRACT_16BITS(tptr+6)&0x4000) ? "Enabled" : "Disabled"
	       ));
	break;

    case LDP_TLV_FEC:
        TLV_TCHECK(1);
        fec_type = *tptr;
	ND_PRINT((ndo, "\n\t      %s FEC (0x%02x)",
	       tok2str(ldp_fec_values, "Unknown", fec_type),
	       fec_type));

	tptr+=1;
	tlv_tlen-=1;
	switch(fec_type) {

	case LDP_FEC_WILDCARD:
	    break;
	case LDP_FEC_PREFIX:
	    TLV_TCHECK(2);
	    af = EXTRACT_16BITS(tptr);
	    tptr+=LDP_TLV_ADDRESS_LIST_AFNUM_LEN;
	    tlv_tlen-=LDP_TLV_ADDRESS_LIST_AFNUM_LEN;
	    if (af == AFNUM_INET) {
		i=decode_prefix4(ndo, tptr, tlv_tlen, buf, sizeof(buf));
		if (i == -2)
		    goto trunc;
		if (i == -3)
		    ND_PRINT((ndo, ": IPv4 prefix (goes past end of TLV)"));
		else if (i == -1)
		    ND_PRINT((ndo, ": IPv4 prefix (invalid length)"));
		else
		    ND_PRINT((ndo, ": IPv4 prefix %s", buf));
	    }
	    else if (af == AFNUM_INET6) {
		i=decode_prefix6(ndo, tptr, tlv_tlen, buf, sizeof(buf));
		if (i == -2)
		    goto trunc;
		if (i == -3)
		    ND_PRINT((ndo, ": IPv4 prefix (goes past end of TLV)"));
		else if (i == -1)
		    ND_PRINT((ndo, ": IPv6 prefix (invalid length)"));
		else
		    ND_PRINT((ndo, ": IPv6 prefix %s", buf));
	    }
	    else
		ND_PRINT((ndo, ": Address family %u prefix", af));
	    break;
	case LDP_FEC_HOSTADDRESS:
	    break;
	case LDP_FEC_MARTINI_VC:
            /*
             * We assume the type was supposed to be one of the MPLS
             * Pseudowire Types.
             */
            TLV_TCHECK(7);
            vc_info_len = *(tptr+2);

            /*
	     * According to RFC 4908, the VC info Length field can be zero,
	     * in which case not only are there no interface parameters,
	     * there's no VC ID.
	     */
            if (vc_info_len == 0) {
                ND_PRINT((ndo, ": %s, %scontrol word, group-ID %u, VC-info-length: %u",
                       tok2str(mpls_pw_types_values, "Unknown", EXTRACT_16BITS(tptr)&0x7fff),
                       EXTRACT_16BITS(tptr)&0x8000 ? "" : "no ",
                       EXTRACT_32BITS(tptr+3),
                       vc_info_len));
                break;
            }

            /* Make sure we have the VC ID as well */
            TLV_TCHECK(11);
	    ND_PRINT((ndo, ": %s, %scontrol word, group-ID %u, VC-ID %u, VC-info-length: %u",
		   tok2str(mpls_pw_types_values, "Unknown", EXTRACT_16BITS(tptr)&0x7fff),
		   EXTRACT_16BITS(tptr)&0x8000 ? "" : "no ",
                   EXTRACT_32BITS(tptr+3),
		   EXTRACT_32BITS(tptr+7),
                   vc_info_len));
            if (vc_info_len < 4) {
                /* minimum 4, for the VC ID */
                ND_PRINT((ndo, " (invalid, < 4"));
                return(tlv_len+4); /* Type & Length fields not included */
	    }
            vc_info_len -= 4; /* subtract out the VC ID, giving the length of the interface parameters */

            /* Skip past the fixed information and the VC ID */
            tptr+=11;
            tlv_tlen-=11;
            TLV_TCHECK(vc_info_len);

            while (vc_info_len > 2) {
                vc_info_tlv_type = *tptr;
                vc_info_tlv_len = *(tptr+1);
                if (vc_info_tlv_len < 2)
                    break;
                if (vc_info_len < vc_info_tlv_len)
                    break;

                ND_PRINT((ndo, "\n\t\tInterface Parameter: %s (0x%02x), len %u",
                       tok2str(ldp_fec_martini_ifparm_values,"Unknown",vc_info_tlv_type),
                       vc_info_tlv_type,
                       vc_info_tlv_len));

                switch(vc_info_tlv_type) {
                case LDP_FEC_MARTINI_IFPARM_MTU:
                    ND_PRINT((ndo, ": %u", EXTRACT_16BITS(tptr+2)));
                    break;

                case LDP_FEC_MARTINI_IFPARM_DESC:
                    ND_PRINT((ndo, ": "));
                    for (idx = 2; idx < vc_info_tlv_len; idx++)
                        safeputchar(ndo, *(tptr + idx));
                    break;

                case LDP_FEC_MARTINI_IFPARM_VCCV:
                    ND_PRINT((ndo, "\n\t\t  Control Channels (0x%02x) = [%s]",
                           *(tptr+2),
                           bittok2str(ldp_fec_martini_ifparm_vccv_cc_values, "none", *(tptr+2))));
                    ND_PRINT((ndo, "\n\t\t  CV Types (0x%02x) = [%s]",
                           *(tptr+3),
                           bittok2str(ldp_fec_martini_ifparm_vccv_cv_values, "none", *(tptr+3))));
                    break;

                default:
                    print_unknown_data(ndo, tptr+2, "\n\t\t  ", vc_info_tlv_len-2);
                    break;
                }

                vc_info_len -= vc_info_tlv_len;
                tptr += vc_info_tlv_len;
            }
	    break;
	}

	break;

    case LDP_TLV_GENERIC_LABEL:
	TLV_TCHECK(4);
	ND_PRINT((ndo, "\n\t      Label: %u", EXTRACT_32BITS(tptr) & 0xfffff));
	break;

    case LDP_TLV_STATUS:
	TLV_TCHECK(8);
	ui = EXTRACT_32BITS(tptr);
	tptr+=4;
	ND_PRINT((ndo, "\n\t      Status: 0x%02x, Flags: [%s and %s forward]",
	       ui&0x3fffffff,
	       ui&0x80000000 ? "Fatal error" : "Advisory Notification",
	       ui&0x40000000 ? "do" : "don't"));
	ui = EXTRACT_32BITS(tptr);
	tptr+=4;
	if (ui)
	    ND_PRINT((ndo, ", causing Message ID: 0x%08x", ui));
 	break;
 
     case LDP_TLV_FT_SESSION:
	TLV_TCHECK(12);
 	ft_flags = EXTRACT_16BITS(tptr);
 	ND_PRINT((ndo, "\n\t      Flags: [%sReconnect, %sSave State, %sAll-Label Protection, %s Checkpoint, %sRe-Learn State]",
 	       ft_flags&0x8000 ? "" : "No ",
 	       ft_flags&0x8 ? "" : "Don't ",
 	       ft_flags&0x4 ? "" : "No ",
 	       ft_flags&0x2 ? "Sequence Numbered Label" : "All Labels",
 	       ft_flags&0x1 ? "" : "Don't "));
	/* 16 bits (FT Flags) + 16 bits (Reserved) */
 	tptr+=4;
 	ui = EXTRACT_32BITS(tptr);
 	if (ui)
	    ND_PRINT((ndo, ", Reconnect Timeout: %ums", ui));
	tptr+=4;
	ui = EXTRACT_32BITS(tptr);
	if (ui)
	    ND_PRINT((ndo, ", Recovery Time: %ums", ui));
	break;

    case LDP_TLV_MTU:
	TLV_TCHECK(2);
	ND_PRINT((ndo, "\n\t      MTU: %u", EXTRACT_16BITS(tptr)));
	break;


    /*
     *  FIXME those are the defined TLVs that lack a decoder
     *  you are welcome to contribute code ;-)
     */

    case LDP_TLV_HOP_COUNT:
    case LDP_TLV_PATH_VECTOR:
    case LDP_TLV_ATM_LABEL:
    case LDP_TLV_FR_LABEL:
    case LDP_TLV_EXTD_STATUS:
    case LDP_TLV_RETURNED_PDU:
    case LDP_TLV_RETURNED_MSG:
    case LDP_TLV_ATM_SESSION_PARM:
    case LDP_TLV_FR_SESSION_PARM:
    case LDP_TLV_LABEL_REQUEST_MSG_ID:

    default:
        if (ndo->ndo_vflag <= 1)
            print_unknown_data(ndo, tptr, "\n\t      ", tlv_tlen);
        break;
    }
     return(tlv_len+4); /* Type & Length fields not included */
 
 trunc:
    ND_PRINT((ndo, "%s", tstr));
     return 0;
 
 badtlv:
    ND_PRINT((ndo, "\n\t\t TLV contents go past end of TLV"));
    return(tlv_len+4); /* Type & Length fields not included */
}
