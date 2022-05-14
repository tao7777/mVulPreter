rsvp_obj_print(netdissect_options *ndo,
               const u_char *pptr, u_int plen, const u_char *tptr,
               const char *ident, u_int tlen,
               const struct rsvp_common_header *rsvp_com_header)
{
    const struct rsvp_object_header *rsvp_obj_header;
    const u_char *obj_tptr;
    union {
        const struct rsvp_obj_integrity_t *rsvp_obj_integrity;
        const struct rsvp_obj_frr_t *rsvp_obj_frr;
    } obj_ptr;

    u_short rsvp_obj_len,rsvp_obj_ctype,obj_tlen,intserv_serv_tlen;
    int hexdump,processed,padbytes,error_code,error_value,i,sigcheck;
    union {
	float f;
	uint32_t i;
    } bw;
    uint8_t namelen;

    u_int action, subchannel;

    while(tlen>=sizeof(struct rsvp_object_header)) {
        /* did we capture enough for fully decoding the object header ? */
        ND_TCHECK2(*tptr, sizeof(struct rsvp_object_header));

        rsvp_obj_header = (const struct rsvp_object_header *)tptr;
        rsvp_obj_len=EXTRACT_16BITS(rsvp_obj_header->length);
        rsvp_obj_ctype=rsvp_obj_header->ctype;

        if(rsvp_obj_len % 4) {
            ND_PRINT((ndo, "%sERROR: object header size %u not a multiple of 4", ident, rsvp_obj_len));
            return -1;
        }
        if(rsvp_obj_len < sizeof(struct rsvp_object_header)) {
            ND_PRINT((ndo, "%sERROR: object header too short %u < %lu", ident, rsvp_obj_len,
                   (unsigned long)sizeof(const struct rsvp_object_header)));
            return -1;
        }

        ND_PRINT((ndo, "%s%s Object (%u) Flags: [%s",
               ident,
               tok2str(rsvp_obj_values,
                       "Unknown",
                       rsvp_obj_header->class_num),
               rsvp_obj_header->class_num,
               ((rsvp_obj_header->class_num) & 0x80) ? "ignore" : "reject"));

        if (rsvp_obj_header->class_num > 128)
            ND_PRINT((ndo, " %s",
                   ((rsvp_obj_header->class_num) & 0x40) ? "and forward" : "silently"));

        ND_PRINT((ndo, " if unknown], Class-Type: %s (%u), length: %u",
               tok2str(rsvp_ctype_values,
                       "Unknown",
                       ((rsvp_obj_header->class_num)<<8)+rsvp_obj_ctype),
               rsvp_obj_ctype,
               rsvp_obj_len));

        if(tlen < rsvp_obj_len) {
            ND_PRINT((ndo, "%sERROR: object goes past end of objects TLV", ident));
            return -1;
        }

        obj_tptr=tptr+sizeof(struct rsvp_object_header);
        obj_tlen=rsvp_obj_len-sizeof(struct rsvp_object_header);

        /* did we capture enough for fully decoding the object ? */
        if (!ND_TTEST2(*tptr, rsvp_obj_len))
            return -1;
        hexdump=FALSE;

        switch(rsvp_obj_header->class_num) {
        case RSVP_OBJ_SESSION:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_IPV4:
                if (obj_tlen < 8)
                    return -1;
                ND_PRINT((ndo, "%s  IPv4 DestAddress: %s, Protocol ID: 0x%02x",
                       ident,
                       ipaddr_string(ndo, obj_tptr),
                       *(obj_tptr + sizeof(struct in_addr))));
                ND_PRINT((ndo, "%s  Flags: [0x%02x], DestPort %u",
                       ident,
                       *(obj_tptr+5),
                       EXTRACT_16BITS(obj_tptr + 6)));
                obj_tlen-=8;
                obj_tptr+=8;
                break;
            case RSVP_CTYPE_IPV6:
                if (obj_tlen < 20)
                    return -1;
                ND_PRINT((ndo, "%s  IPv6 DestAddress: %s, Protocol ID: 0x%02x",
                       ident,
                       ip6addr_string(ndo, obj_tptr),
                       *(obj_tptr + sizeof(struct in6_addr))));
                ND_PRINT((ndo, "%s  Flags: [0x%02x], DestPort %u",
                       ident,
                       *(obj_tptr+sizeof(struct in6_addr)+1),
                       EXTRACT_16BITS(obj_tptr + sizeof(struct in6_addr) + 2)));
                obj_tlen-=20;
                obj_tptr+=20;
                break;

            case RSVP_CTYPE_TUNNEL_IPV6:
                if (obj_tlen < 36)
                    return -1;
                ND_PRINT((ndo, "%s  IPv6 Tunnel EndPoint: %s, Tunnel ID: 0x%04x, Extended Tunnel ID: %s",
                       ident,
                       ip6addr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr+18),
                       ip6addr_string(ndo, obj_tptr + 20)));
                obj_tlen-=36;
                obj_tptr+=36;
                break;

            case RSVP_CTYPE_14: /* IPv6 p2mp LSP Tunnel */
                if (obj_tlen < 26)
                    return -1;
                ND_PRINT((ndo, "%s  IPv6 P2MP LSP ID: 0x%08x, Tunnel ID: 0x%04x, Extended Tunnel ID: %s",
                       ident,
                       EXTRACT_32BITS(obj_tptr),
                       EXTRACT_16BITS(obj_tptr+6),
                       ip6addr_string(ndo, obj_tptr + 8)));
                obj_tlen-=26;
                obj_tptr+=26;
                break;
            case RSVP_CTYPE_13: /* IPv4 p2mp LSP Tunnel */
                if (obj_tlen < 12)
                    return -1;
                ND_PRINT((ndo, "%s  IPv4 P2MP LSP ID: %s, Tunnel ID: 0x%04x, Extended Tunnel ID: %s",
                       ident,
                       ipaddr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr+6),
                       ipaddr_string(ndo, obj_tptr + 8)));
                obj_tlen-=12;
                obj_tptr+=12;
                break;
            case RSVP_CTYPE_TUNNEL_IPV4:
            case RSVP_CTYPE_UNI_IPV4:
                if (obj_tlen < 12)
                    return -1;
                ND_PRINT((ndo, "%s  IPv4 Tunnel EndPoint: %s, Tunnel ID: 0x%04x, Extended Tunnel ID: %s",
                       ident,
                       ipaddr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr+6),
                       ipaddr_string(ndo, obj_tptr + 8)));
                obj_tlen-=12;
                obj_tptr+=12;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_CONFIRM:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_IPV4:
                if (obj_tlen < sizeof(struct in_addr))
                    return -1;
                ND_PRINT((ndo, "%s  IPv4 Receiver Address: %s",
                       ident,
                       ipaddr_string(ndo, obj_tptr)));
                obj_tlen-=sizeof(struct in_addr);
                obj_tptr+=sizeof(struct in_addr);
                break;
            case RSVP_CTYPE_IPV6:
                if (obj_tlen < sizeof(struct in6_addr))
                    return -1;
                ND_PRINT((ndo, "%s  IPv6 Receiver Address: %s",
                       ident,
                       ip6addr_string(ndo, obj_tptr)));
                obj_tlen-=sizeof(struct in6_addr);
                obj_tptr+=sizeof(struct in6_addr);
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_NOTIFY_REQ:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_IPV4:
                if (obj_tlen < sizeof(struct in_addr))
                    return -1;
                ND_PRINT((ndo, "%s  IPv4 Notify Node Address: %s",
                       ident,
                       ipaddr_string(ndo, obj_tptr)));
                obj_tlen-=sizeof(struct in_addr);
                obj_tptr+=sizeof(struct in_addr);
                break;
            case RSVP_CTYPE_IPV6:
                if (obj_tlen < sizeof(struct in6_addr))
                    return-1;
                ND_PRINT((ndo, "%s  IPv6 Notify Node Address: %s",
                       ident,
                       ip6addr_string(ndo, obj_tptr)));
                obj_tlen-=sizeof(struct in6_addr);
                obj_tptr+=sizeof(struct in6_addr);
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_SUGGESTED_LABEL: /* fall through */
        case RSVP_OBJ_UPSTREAM_LABEL:  /* fall through */
        case RSVP_OBJ_RECOVERY_LABEL:  /* fall through */
        case RSVP_OBJ_LABEL:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                while(obj_tlen >= 4 ) {
                    ND_PRINT((ndo, "%s  Label: %u", ident, EXTRACT_32BITS(obj_tptr)));
                    obj_tlen-=4;
                    obj_tptr+=4;
                }
                break;
            case RSVP_CTYPE_2:
                if (obj_tlen < 4)
                    return-1;
                ND_PRINT((ndo, "%s  Generalized Label: %u",
                       ident,
                       EXTRACT_32BITS(obj_tptr)));
                obj_tlen-=4;
                obj_tptr+=4;
                break;
            case RSVP_CTYPE_3:
                if (obj_tlen < 12)
                    return-1;
                ND_PRINT((ndo, "%s  Waveband ID: %u%s  Start Label: %u, Stop Label: %u",
                       ident,
                       EXTRACT_32BITS(obj_tptr),
                       ident,
                       EXTRACT_32BITS(obj_tptr+4),
                       EXTRACT_32BITS(obj_tptr + 8)));
                obj_tlen-=12;
                obj_tptr+=12;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_STYLE:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                if (obj_tlen < 4)
                    return-1;
                ND_PRINT((ndo, "%s  Reservation Style: %s, Flags: [0x%02x]",
                       ident,
                       tok2str(rsvp_resstyle_values,
                               "Unknown",
                               EXTRACT_24BITS(obj_tptr+1)),
                       *(obj_tptr)));
                obj_tlen-=4;
                obj_tptr+=4;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_SENDER_TEMPLATE:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_IPV4:
                if (obj_tlen < 8)
                    return-1;
                ND_PRINT((ndo, "%s  Source Address: %s, Source Port: %u",
                       ident,
                       ipaddr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr + 6)));
                obj_tlen-=8;
                obj_tptr+=8;
                break;
            case RSVP_CTYPE_IPV6:
                if (obj_tlen < 20)
                    return-1;
                ND_PRINT((ndo, "%s  Source Address: %s, Source Port: %u",
                       ident,
                       ip6addr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr + 18)));
                obj_tlen-=20;
                obj_tptr+=20;
                break;
            case RSVP_CTYPE_13: /* IPv6 p2mp LSP tunnel */
                if (obj_tlen < 40)
                    return-1;
                ND_PRINT((ndo, "%s  IPv6 Tunnel Sender Address: %s, LSP ID: 0x%04x"
                       "%s  Sub-Group Originator ID: %s, Sub-Group ID: 0x%04x",
                       ident,
                       ip6addr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr+18),
                       ident,
                       ip6addr_string(ndo, obj_tptr+20),
                       EXTRACT_16BITS(obj_tptr + 38)));
                obj_tlen-=40;
                obj_tptr+=40;
                break;
            case RSVP_CTYPE_TUNNEL_IPV4:
                if (obj_tlen < 8)
                    return-1;
                ND_PRINT((ndo, "%s  IPv4 Tunnel Sender Address: %s, LSP-ID: 0x%04x",
                       ident,
                       ipaddr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr + 6)));
                obj_tlen-=8;
                obj_tptr+=8;
                break;
            case RSVP_CTYPE_12: /* IPv4 p2mp LSP tunnel */
                if (obj_tlen < 16)
                    return-1;
                ND_PRINT((ndo, "%s  IPv4 Tunnel Sender Address: %s, LSP ID: 0x%04x"
                       "%s  Sub-Group Originator ID: %s, Sub-Group ID: 0x%04x",
                       ident,
                       ipaddr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr+6),
                       ident,
                       ipaddr_string(ndo, obj_tptr+8),
                       EXTRACT_16BITS(obj_tptr + 12)));
                obj_tlen-=16;
                obj_tptr+=16;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_LABEL_REQ:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                while(obj_tlen >= 4 ) {
                    ND_PRINT((ndo, "%s  L3 Protocol ID: %s",
                           ident,
                           tok2str(ethertype_values,
                                   "Unknown Protocol (0x%04x)",
                                   EXTRACT_16BITS(obj_tptr + 2))));
                    obj_tlen-=4;
                    obj_tptr+=4;
                }
                break;
            case RSVP_CTYPE_2:
                if (obj_tlen < 12)
                    return-1;
                ND_PRINT((ndo, "%s  L3 Protocol ID: %s",
                       ident,
                       tok2str(ethertype_values,
                               "Unknown Protocol (0x%04x)",
                               EXTRACT_16BITS(obj_tptr + 2))));
                ND_PRINT((ndo, ",%s merge capability",((*(obj_tptr + 4)) & 0x80) ? "no" : "" ));
                ND_PRINT((ndo, "%s  Minimum VPI/VCI: %u/%u",
                       ident,
                       (EXTRACT_16BITS(obj_tptr+4))&0xfff,
                       (EXTRACT_16BITS(obj_tptr + 6)) & 0xfff));
                ND_PRINT((ndo, "%s  Maximum VPI/VCI: %u/%u",
                       ident,
                       (EXTRACT_16BITS(obj_tptr+8))&0xfff,
                       (EXTRACT_16BITS(obj_tptr + 10)) & 0xfff));
                obj_tlen-=12;
                obj_tptr+=12;
                break;
            case RSVP_CTYPE_3:
                if (obj_tlen < 12)
                    return-1;
                ND_PRINT((ndo, "%s  L3 Protocol ID: %s",
                       ident,
                       tok2str(ethertype_values,
                               "Unknown Protocol (0x%04x)",
                               EXTRACT_16BITS(obj_tptr + 2))));
                ND_PRINT((ndo, "%s  Minimum/Maximum DLCI: %u/%u, %s%s bit DLCI",
                       ident,
                       (EXTRACT_32BITS(obj_tptr+4))&0x7fffff,
                       (EXTRACT_32BITS(obj_tptr+8))&0x7fffff,
                       (((EXTRACT_16BITS(obj_tptr+4)>>7)&3) == 0 ) ? "10" : "",
                       (((EXTRACT_16BITS(obj_tptr + 4) >> 7) & 3) == 2 ) ? "23" : ""));
                obj_tlen-=12;
                obj_tptr+=12;
                break;
            case RSVP_CTYPE_4:
                if (obj_tlen < 4)
                    return-1;
                ND_PRINT((ndo, "%s  LSP Encoding Type: %s (%u)",
                       ident,
                       tok2str(gmpls_encoding_values,
                               "Unknown",
                               *obj_tptr),
		       *obj_tptr));
                ND_PRINT((ndo, "%s  Switching Type: %s (%u), Payload ID: %s (0x%04x)",
                       ident,
                       tok2str(gmpls_switch_cap_values,
                               "Unknown",
                               *(obj_tptr+1)),
		       *(obj_tptr+1),
                       tok2str(gmpls_payload_values,
                               "Unknown",
                               EXTRACT_16BITS(obj_tptr+2)),
		       EXTRACT_16BITS(obj_tptr + 2)));
                obj_tlen-=4;
                obj_tptr+=4;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_RRO:
        case RSVP_OBJ_ERO:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_IPV4:
                while(obj_tlen >= 4 ) {
		    u_char length;

		    ND_TCHECK2(*obj_tptr, 4);
		    length = *(obj_tptr + 1);
                    ND_PRINT((ndo, "%s  Subobject Type: %s, length %u",
                           ident,
                           tok2str(rsvp_obj_xro_values,
                                   "Unknown %u",
                                   RSVP_OBJ_XRO_MASK_SUBOBJ(*obj_tptr)),
                           length));

                    if (length == 0) { /* prevent infinite loops */
                        ND_PRINT((ndo, "%s  ERROR: zero length ERO subtype", ident));
                        break;
                    }

                    switch(RSVP_OBJ_XRO_MASK_SUBOBJ(*obj_tptr)) {
		    u_char prefix_length;

                    case RSVP_OBJ_XRO_IPV4:
			if (length != 8) {
				ND_PRINT((ndo, " ERROR: length != 8"));
				goto invalid;
			}
			ND_TCHECK2(*obj_tptr, 8);
			prefix_length = *(obj_tptr+6);
			if (prefix_length != 32) {
				ND_PRINT((ndo, " ERROR: Prefix length %u != 32",
					  prefix_length));
				goto invalid;
			}
                        ND_PRINT((ndo, ", %s, %s/%u, Flags: [%s]",
                               RSVP_OBJ_XRO_MASK_LOOSE(*obj_tptr) ? "Loose" : "Strict",
                               ipaddr_string(ndo, obj_tptr+2),
                               *(obj_tptr+6),
                               bittok2str(rsvp_obj_rro_flag_values,
                                   "none",
                                   *(obj_tptr + 7)))); /* rfc3209 says that this field is rsvd. */
                    break;
                    case RSVP_OBJ_XRO_LABEL:
			if (length != 8) {
				ND_PRINT((ndo, " ERROR: length != 8"));
				goto invalid;
			}
			ND_TCHECK2(*obj_tptr, 8);
                        ND_PRINT((ndo, ", Flags: [%s] (%#x), Class-Type: %s (%u), %u",
                               bittok2str(rsvp_obj_rro_label_flag_values,
                                   "none",
                                   *(obj_tptr+2)),
                               *(obj_tptr+2),
                               tok2str(rsvp_ctype_values,
                                       "Unknown",
                                       *(obj_tptr+3) + 256*RSVP_OBJ_RRO),
                               *(obj_tptr+3),
                               EXTRACT_32BITS(obj_tptr + 4)));
                    }
                    obj_tlen-=*(obj_tptr+1);
                    obj_tptr+=*(obj_tptr+1);
                }
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_HELLO:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
            case RSVP_CTYPE_2:
                if (obj_tlen < 8)
                    return-1;
                ND_PRINT((ndo, "%s  Source Instance: 0x%08x, Destination Instance: 0x%08x",
                       ident,
                       EXTRACT_32BITS(obj_tptr),
                       EXTRACT_32BITS(obj_tptr + 4)));
                obj_tlen-=8;
                obj_tptr+=8;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_RESTART_CAPABILITY:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                if (obj_tlen < 8)
                    return-1;
                ND_PRINT((ndo, "%s  Restart  Time: %ums, Recovery Time: %ums",
                       ident,
                       EXTRACT_32BITS(obj_tptr),
                       EXTRACT_32BITS(obj_tptr + 4)));
                obj_tlen-=8;
                obj_tptr+=8;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_SESSION_ATTRIBUTE:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_TUNNEL_IPV4:
                if (obj_tlen < 4)
                    return-1;
                namelen = *(obj_tptr+3);
                if (obj_tlen < 4+namelen)
                    return-1;
                ND_PRINT((ndo, "%s  Session Name: ", ident));
                for (i = 0; i < namelen; i++)
                    safeputchar(ndo, *(obj_tptr + 4 + i));
                ND_PRINT((ndo, "%s  Setup Priority: %u, Holding Priority: %u, Flags: [%s] (%#x)",
                       ident,
                       (int)*obj_tptr,
                       (int)*(obj_tptr+1),
                       bittok2str(rsvp_session_attribute_flag_values,
                                  "none",
                                  *(obj_tptr+2)),
                       *(obj_tptr + 2)));
                obj_tlen-=4+*(obj_tptr+3);
                obj_tptr+=4+*(obj_tptr+3);
                break;
            default:
                hexdump=TRUE;
            }
            break;

	case RSVP_OBJ_GENERALIZED_UNI:
            switch(rsvp_obj_ctype) {
		int subobj_type,af,subobj_len,total_subobj_len;

            case RSVP_CTYPE_1:

                if (obj_tlen < 4)
                    return-1;

		/* read variable length subobjects */
		total_subobj_len = obj_tlen;
                while(total_subobj_len > 0) {
                    /* If RFC 3476 Section 3.1 defined that a sub-object of the
                     * GENERALIZED_UNI RSVP object must have the Length field as
                     * a multiple of 4, instead of the check below it would be
                     * better to test total_subobj_len only once before the loop.
                     * So long as it does not define it and this while loop does
                     * not implement such a requirement, let's accept that within
                     * each iteration subobj_len may happen to be a multiple of 1
                     * and test it and total_subobj_len respectively.
                     */
                    if (total_subobj_len < 4)
                        goto invalid;
                    subobj_len  = EXTRACT_16BITS(obj_tptr);
                    subobj_type = (EXTRACT_16BITS(obj_tptr+2))>>8;
                    af = (EXTRACT_16BITS(obj_tptr+2))&0x00FF;

                    ND_PRINT((ndo, "%s  Subobject Type: %s (%u), AF: %s (%u), length: %u",
                           ident,
                           tok2str(rsvp_obj_generalized_uni_values, "Unknown", subobj_type),
                           subobj_type,
                           tok2str(af_values, "Unknown", af), af,
                           subobj_len));

                    /* In addition to what is explained above, the same spec does not
                     * explicitly say that the same Length field includes the 4-octet
                     * sub-object header, but as long as this while loop implements it
                     * as it does include, let's keep the check below consistent with
                     * the rest of the code.
                     */
                    if(subobj_len < 4 || subobj_len > total_subobj_len)
                        goto invalid;

                    switch(subobj_type) {
                    case RSVP_GEN_UNI_SUBOBJ_SOURCE_TNA_ADDRESS:
                    case RSVP_GEN_UNI_SUBOBJ_DESTINATION_TNA_ADDRESS:

                        switch(af) {
                        case AFNUM_INET:
                            if (subobj_len < 8)
                                return -1;
                            ND_PRINT((ndo, "%s    UNI IPv4 TNA address: %s",
                                   ident, ipaddr_string(ndo, obj_tptr + 4)));
                            break;
                        case AFNUM_INET6:
                            if (subobj_len < 20)
                                return -1;
                            ND_PRINT((ndo, "%s    UNI IPv6 TNA address: %s",
                                   ident, ip6addr_string(ndo, obj_tptr + 4)));
                            break;
                        case AFNUM_NSAP:
                            if (subobj_len) {
                                /* unless we have a TLV parser lets just hexdump */
                                hexdump=TRUE;
                            }
                            break;
                        }
                        break;

                    case RSVP_GEN_UNI_SUBOBJ_DIVERSITY:
                        if (subobj_len) {
                            /* unless we have a TLV parser lets just hexdump */
                            hexdump=TRUE;
                        }
                        break;

                    case RSVP_GEN_UNI_SUBOBJ_EGRESS_LABEL:
                        if (subobj_len < 16) {
                            return -1;
                        }

                        ND_PRINT((ndo, "%s    U-bit: %x, Label type: %u, Logical port id: %u, Label: %u",
                               ident,
                               ((EXTRACT_32BITS(obj_tptr+4))>>31),
                               ((EXTRACT_32BITS(obj_tptr+4))&0xFF),
                               EXTRACT_32BITS(obj_tptr+8),
                               EXTRACT_32BITS(obj_tptr + 12)));
                        break;

                    case RSVP_GEN_UNI_SUBOBJ_SERVICE_LEVEL:
                        if (subobj_len < 8) {
                            return -1;
                        }

                        ND_PRINT((ndo, "%s    Service level: %u",
                               ident, (EXTRACT_32BITS(obj_tptr + 4)) >> 24));
                        break;

                    default:
                        hexdump=TRUE;
                        break;
                    }
                    total_subobj_len-=subobj_len;
                    obj_tptr+=subobj_len;
                    obj_tlen+=subobj_len;
		}

                if (total_subobj_len) {
                    /* unless we have a TLV parser lets just hexdump */
                    hexdump=TRUE;
                }
                break;

            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_RSVP_HOP:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_3: /* fall through - FIXME add TLV parser */
            case RSVP_CTYPE_IPV4:
                if (obj_tlen < 8)
                    return-1;
                ND_PRINT((ndo, "%s  Previous/Next Interface: %s, Logical Interface Handle: 0x%08x",
                       ident,
                       ipaddr_string(ndo, obj_tptr),
                       EXTRACT_32BITS(obj_tptr + 4)));
                obj_tlen-=8;
                obj_tptr+=8;
                if (obj_tlen)
                    hexdump=TRUE; /* unless we have a TLV parser lets just hexdump */
                break;
            case RSVP_CTYPE_4: /* fall through - FIXME add TLV parser */
            case RSVP_CTYPE_IPV6:
                if (obj_tlen < 20)
                    return-1;
                ND_PRINT((ndo, "%s  Previous/Next Interface: %s, Logical Interface Handle: 0x%08x",
                       ident,
                       ip6addr_string(ndo, obj_tptr),
                       EXTRACT_32BITS(obj_tptr + 16)));
                obj_tlen-=20;
                obj_tptr+=20;
                hexdump=TRUE; /* unless we have a TLV parser lets just hexdump */
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_TIME_VALUES:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                if (obj_tlen < 4)
                    return-1;
                ND_PRINT((ndo, "%s  Refresh Period: %ums",
                       ident,
                       EXTRACT_32BITS(obj_tptr)));
                obj_tlen-=4;
                obj_tptr+=4;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        /* those three objects do share the same semantics */
        case RSVP_OBJ_SENDER_TSPEC:
        case RSVP_OBJ_ADSPEC:
        case RSVP_OBJ_FLOWSPEC:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_2:
                if (obj_tlen < 4)
                    return-1;
                ND_PRINT((ndo, "%s  Msg-Version: %u, length: %u",
                       ident,
                       (*obj_tptr & 0xf0) >> 4,
                       EXTRACT_16BITS(obj_tptr + 2) << 2));
                obj_tptr+=4; /* get to the start of the service header */
                obj_tlen-=4;

                while (obj_tlen >= 4) {
                    intserv_serv_tlen=EXTRACT_16BITS(obj_tptr+2)<<2;
                    ND_PRINT((ndo, "%s  Service Type: %s (%u), break bit %s set, Service length: %u",
                           ident,
                           tok2str(rsvp_intserv_service_type_values,"unknown",*(obj_tptr)),
                           *(obj_tptr),
                           (*(obj_tptr+1)&0x80) ? "" : "not",
                           intserv_serv_tlen));

                    obj_tptr+=4; /* get to the start of the parameter list */
                    obj_tlen-=4;

                    while (intserv_serv_tlen>=4) {
                        processed = rsvp_intserv_print(ndo, obj_tptr, obj_tlen);
                        if (processed == 0)
                            break;
                        obj_tlen-=processed;
                        intserv_serv_tlen-=processed;
                        obj_tptr+=processed;
                    }
                }
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_FILTERSPEC:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_IPV4:
                if (obj_tlen < 8)
                    return-1;
                ND_PRINT((ndo, "%s  Source Address: %s, Source Port: %u",
                       ident,
                       ipaddr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr + 6)));
                obj_tlen-=8;
                obj_tptr+=8;
                break;
            case RSVP_CTYPE_IPV6:
                if (obj_tlen < 20)
                    return-1;
                ND_PRINT((ndo, "%s  Source Address: %s, Source Port: %u",
                       ident,
                       ip6addr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr + 18)));
                obj_tlen-=20;
                obj_tptr+=20;
                break;
            case RSVP_CTYPE_3:
                if (obj_tlen < 20)
                    return-1;
                ND_PRINT((ndo, "%s  Source Address: %s, Flow Label: %u",
                       ident,
                       ip6addr_string(ndo, obj_tptr),
                       EXTRACT_24BITS(obj_tptr + 17)));
                obj_tlen-=20;
                obj_tptr+=20;
                break;
            case RSVP_CTYPE_TUNNEL_IPV6:
                if (obj_tlen < 20)
                    return-1;
                ND_PRINT((ndo, "%s  Source Address: %s, LSP-ID: 0x%04x",
                       ident,
                       ipaddr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr + 18)));
                obj_tlen-=20;
                obj_tptr+=20;
                break;
            case RSVP_CTYPE_13: /* IPv6 p2mp LSP tunnel */
                if (obj_tlen < 40)
                    return-1;
                ND_PRINT((ndo, "%s  IPv6 Tunnel Sender Address: %s, LSP ID: 0x%04x"
                       "%s  Sub-Group Originator ID: %s, Sub-Group ID: 0x%04x",
                       ident,
                       ip6addr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr+18),
                       ident,
                       ip6addr_string(ndo, obj_tptr+20),
                       EXTRACT_16BITS(obj_tptr + 38)));
                obj_tlen-=40;
                obj_tptr+=40;
                break;
            case RSVP_CTYPE_TUNNEL_IPV4:
                if (obj_tlen < 8)
                    return-1;
                ND_PRINT((ndo, "%s  Source Address: %s, LSP-ID: 0x%04x",
                       ident,
                       ipaddr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr + 6)));
                obj_tlen-=8;
                obj_tptr+=8;
                break;
            case RSVP_CTYPE_12: /* IPv4 p2mp LSP tunnel */
                if (obj_tlen < 16)
                    return-1;
                ND_PRINT((ndo, "%s  IPv4 Tunnel Sender Address: %s, LSP ID: 0x%04x"
                       "%s  Sub-Group Originator ID: %s, Sub-Group ID: 0x%04x",
                       ident,
                       ipaddr_string(ndo, obj_tptr),
                       EXTRACT_16BITS(obj_tptr+6),
                       ident,
                       ipaddr_string(ndo, obj_tptr+8),
                       EXTRACT_16BITS(obj_tptr + 12)));
                obj_tlen-=16;
                obj_tptr+=16;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_FASTREROUTE:
            /* the differences between c-type 1 and 7 are minor */
            obj_ptr.rsvp_obj_frr = (const struct rsvp_obj_frr_t *)obj_tptr;

            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1: /* new style */
                if (obj_tlen < sizeof(struct rsvp_obj_frr_t))
                    return-1;
                bw.i = EXTRACT_32BITS(obj_ptr.rsvp_obj_frr->bandwidth);
                ND_PRINT((ndo, "%s  Setup Priority: %u, Holding Priority: %u, Hop-limit: %u, Bandwidth: %.10g Mbps",
                       ident,
                       (int)obj_ptr.rsvp_obj_frr->setup_prio,
                       (int)obj_ptr.rsvp_obj_frr->hold_prio,
                       (int)obj_ptr.rsvp_obj_frr->hop_limit,
                        bw.f * 8 / 1000000));
                ND_PRINT((ndo, "%s  Include-any: 0x%08x, Exclude-any: 0x%08x, Include-all: 0x%08x",
                       ident,
                       EXTRACT_32BITS(obj_ptr.rsvp_obj_frr->include_any),
                       EXTRACT_32BITS(obj_ptr.rsvp_obj_frr->exclude_any),
                       EXTRACT_32BITS(obj_ptr.rsvp_obj_frr->include_all)));
                obj_tlen-=sizeof(struct rsvp_obj_frr_t);
                obj_tptr+=sizeof(struct rsvp_obj_frr_t);
                break;

            case RSVP_CTYPE_TUNNEL_IPV4: /* old style */
                if (obj_tlen < 16)
                    return-1;
                bw.i = EXTRACT_32BITS(obj_ptr.rsvp_obj_frr->bandwidth);
                ND_PRINT((ndo, "%s  Setup Priority: %u, Holding Priority: %u, Hop-limit: %u, Bandwidth: %.10g Mbps",
                       ident,
                       (int)obj_ptr.rsvp_obj_frr->setup_prio,
                       (int)obj_ptr.rsvp_obj_frr->hold_prio,
                       (int)obj_ptr.rsvp_obj_frr->hop_limit,
                        bw.f * 8 / 1000000));
                ND_PRINT((ndo, "%s  Include Colors: 0x%08x, Exclude Colors: 0x%08x",
                       ident,
                       EXTRACT_32BITS(obj_ptr.rsvp_obj_frr->include_any),
                       EXTRACT_32BITS(obj_ptr.rsvp_obj_frr->exclude_any)));
                obj_tlen-=16;
                obj_tptr+=16;
                break;

            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_DETOUR:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_TUNNEL_IPV4:
                while(obj_tlen >= 8) {
                    ND_PRINT((ndo, "%s  PLR-ID: %s, Avoid-Node-ID: %s",
                           ident,
                           ipaddr_string(ndo, obj_tptr),
                           ipaddr_string(ndo, obj_tptr + 4)));
                    obj_tlen-=8;
                    obj_tptr+=8;
                }
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_CLASSTYPE:
         case RSVP_OBJ_CLASSTYPE_OLD: /* fall through */
             switch(rsvp_obj_ctype) {
             case RSVP_CTYPE_1:
                ND_TCHECK_32BITS(obj_tptr);
                 ND_PRINT((ndo, "%s  CT: %u",
                        ident,
                        EXTRACT_32BITS(obj_tptr) & 0x7));
                obj_tlen-=4;
                obj_tptr+=4;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_ERROR_SPEC:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_3: /* fall through - FIXME add TLV parser */
            case RSVP_CTYPE_IPV4:
                if (obj_tlen < 8)
                    return-1;
                error_code=*(obj_tptr+5);
                error_value=EXTRACT_16BITS(obj_tptr+6);
                ND_PRINT((ndo, "%s  Error Node Address: %s, Flags: [0x%02x]%s  Error Code: %s (%u)",
                       ident,
                       ipaddr_string(ndo, obj_tptr),
                       *(obj_tptr+4),
                       ident,
                       tok2str(rsvp_obj_error_code_values,"unknown",error_code),
                       error_code));
                switch (error_code) {
                case RSVP_OBJ_ERROR_SPEC_CODE_ROUTING:
                    ND_PRINT((ndo, ", Error Value: %s (%u)",
                           tok2str(rsvp_obj_error_code_routing_values,"unknown",error_value),
                           error_value));
                    break;
                case RSVP_OBJ_ERROR_SPEC_CODE_DIFFSERV_TE: /* fall through */
                case RSVP_OBJ_ERROR_SPEC_CODE_DIFFSERV_TE_OLD:
                    ND_PRINT((ndo, ", Error Value: %s (%u)",
                           tok2str(rsvp_obj_error_code_diffserv_te_values,"unknown",error_value),
                           error_value));
                    break;
                default:
                    ND_PRINT((ndo, ", Unknown Error Value (%u)", error_value));
                    break;
                }
                obj_tlen-=8;
                obj_tptr+=8;
                break;
            case RSVP_CTYPE_4: /* fall through - FIXME add TLV parser */
            case RSVP_CTYPE_IPV6:
                if (obj_tlen < 20)
                    return-1;
                error_code=*(obj_tptr+17);
                error_value=EXTRACT_16BITS(obj_tptr+18);
                ND_PRINT((ndo, "%s  Error Node Address: %s, Flags: [0x%02x]%s  Error Code: %s (%u)",
                       ident,
                       ip6addr_string(ndo, obj_tptr),
                       *(obj_tptr+16),
                       ident,
                       tok2str(rsvp_obj_error_code_values,"unknown",error_code),
                       error_code));

                switch (error_code) {
                case RSVP_OBJ_ERROR_SPEC_CODE_ROUTING:
                    ND_PRINT((ndo, ", Error Value: %s (%u)",
                           tok2str(rsvp_obj_error_code_routing_values,"unknown",error_value),
			   error_value));
                    break;
                default:
                    break;
                }
                obj_tlen-=20;
                obj_tptr+=20;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_PROPERTIES:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                if (obj_tlen < 4)
                    return-1;
                padbytes = EXTRACT_16BITS(obj_tptr+2);
                ND_PRINT((ndo, "%s  TLV count: %u, padding bytes: %u",
                       ident,
                       EXTRACT_16BITS(obj_tptr),
                       padbytes));
                obj_tlen-=4;
                obj_tptr+=4;
                /* loop through as long there is anything longer than the TLV header (2) */
                while(obj_tlen >= 2 + padbytes) {
                    ND_PRINT((ndo, "%s    %s TLV (0x%02x), length: %u", /* length includes header */
                           ident,
                           tok2str(rsvp_obj_prop_tlv_values,"unknown",*obj_tptr),
                           *obj_tptr,
                           *(obj_tptr + 1)));
                    if (obj_tlen < *(obj_tptr+1))
                        return-1;
                    if (*(obj_tptr+1) < 2)
                        return -1;
                    print_unknown_data(ndo, obj_tptr + 2, "\n\t\t", *(obj_tptr + 1) - 2);
                    obj_tlen-=*(obj_tptr+1);
                    obj_tptr+=*(obj_tptr+1);
                }
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_MESSAGE_ID:     /* fall through */
        case RSVP_OBJ_MESSAGE_ID_ACK: /* fall through */
        case RSVP_OBJ_MESSAGE_ID_LIST:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
            case RSVP_CTYPE_2:
                if (obj_tlen < 8)
                    return-1;
                ND_PRINT((ndo, "%s  Flags [0x%02x], epoch: %u",
                       ident,
                       *obj_tptr,
                       EXTRACT_24BITS(obj_tptr + 1)));
                obj_tlen-=4;
                obj_tptr+=4;
                /* loop through as long there are no messages left */
                while(obj_tlen >= 4) {
                    ND_PRINT((ndo, "%s    Message-ID 0x%08x (%u)",
                           ident,
                           EXTRACT_32BITS(obj_tptr),
                           EXTRACT_32BITS(obj_tptr)));
                    obj_tlen-=4;
                    obj_tptr+=4;
                }
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_INTEGRITY:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                if (obj_tlen < sizeof(struct rsvp_obj_integrity_t))
                    return-1;
                obj_ptr.rsvp_obj_integrity = (const struct rsvp_obj_integrity_t *)obj_tptr;
                ND_PRINT((ndo, "%s  Key-ID 0x%04x%08x, Sequence 0x%08x%08x, Flags [%s]",
                       ident,
                       EXTRACT_16BITS(obj_ptr.rsvp_obj_integrity->key_id),
                       EXTRACT_32BITS(obj_ptr.rsvp_obj_integrity->key_id+2),
                       EXTRACT_32BITS(obj_ptr.rsvp_obj_integrity->sequence),
                       EXTRACT_32BITS(obj_ptr.rsvp_obj_integrity->sequence+4),
                       bittok2str(rsvp_obj_integrity_flag_values,
                                  "none",
                                  obj_ptr.rsvp_obj_integrity->flags)));
                ND_PRINT((ndo, "%s  MD5-sum 0x%08x%08x%08x%08x ",
                       ident,
                       EXTRACT_32BITS(obj_ptr.rsvp_obj_integrity->digest),
                       EXTRACT_32BITS(obj_ptr.rsvp_obj_integrity->digest+4),
                       EXTRACT_32BITS(obj_ptr.rsvp_obj_integrity->digest+8),
                       EXTRACT_32BITS(obj_ptr.rsvp_obj_integrity->digest + 12)));

                sigcheck = signature_verify(ndo, pptr, plen,
                                            obj_ptr.rsvp_obj_integrity->digest,
                                            rsvp_clear_checksum,
                                            rsvp_com_header);
                ND_PRINT((ndo, " (%s)", tok2str(signature_check_values, "Unknown", sigcheck)));

                obj_tlen+=sizeof(struct rsvp_obj_integrity_t);
                obj_tptr+=sizeof(struct rsvp_obj_integrity_t);
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_ADMIN_STATUS:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                if (obj_tlen < 4)
                    return-1;
                ND_PRINT((ndo, "%s  Flags [%s]", ident,
                       bittok2str(rsvp_obj_admin_status_flag_values, "none",
                                  EXTRACT_32BITS(obj_tptr))));
                obj_tlen-=4;
                obj_tptr+=4;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_LABEL_SET:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                if (obj_tlen < 4)
                    return-1;
                action = (EXTRACT_16BITS(obj_tptr)>>8);

                ND_PRINT((ndo, "%s  Action: %s (%u), Label type: %u", ident,
                       tok2str(rsvp_obj_label_set_action_values, "Unknown", action),
                       action, ((EXTRACT_32BITS(obj_tptr) & 0x7F))));

                switch (action) {
                case LABEL_SET_INCLUSIVE_RANGE:
                case LABEL_SET_EXCLUSIVE_RANGE: /* fall through */

		    /* only a couple of subchannels are expected */
		    if (obj_tlen < 12)
			return -1;
		    ND_PRINT((ndo, "%s  Start range: %u, End range: %u", ident,
                           EXTRACT_32BITS(obj_tptr+4),
                           EXTRACT_32BITS(obj_tptr + 8)));
		    obj_tlen-=12;
		    obj_tptr+=12;
                    break;

                default:
                    obj_tlen-=4;
                    obj_tptr+=4;
                    subchannel = 1;
                    while(obj_tlen >= 4 ) {
                        ND_PRINT((ndo, "%s  Subchannel #%u: %u", ident, subchannel,
                               EXTRACT_32BITS(obj_tptr)));
                        obj_tptr+=4;
                        obj_tlen-=4;
                        subchannel++;
                    }
                    break;
                }
                break;
            default:
                hexdump=TRUE;
            }

        case RSVP_OBJ_S2L:
            switch (rsvp_obj_ctype) {
            case RSVP_CTYPE_IPV4:
                if (obj_tlen < 4)
                    return-1;
                ND_PRINT((ndo, "%s  Sub-LSP destination address: %s",
                       ident, ipaddr_string(ndo, obj_tptr)));

                obj_tlen-=4;
                obj_tptr+=4;
                break;
            case RSVP_CTYPE_IPV6:
                if (obj_tlen < 16)
                    return-1;
                ND_PRINT((ndo, "%s  Sub-LSP destination address: %s",
                       ident, ip6addr_string(ndo, obj_tptr)));

                obj_tlen-=16;
                obj_tptr+=16;
                break;
            default:
                hexdump=TRUE;
            }

        /*
         *  FIXME those are the defined objects that lack a decoder
         *  you are welcome to contribute code ;-)
         */

        case RSVP_OBJ_SCOPE:
        case RSVP_OBJ_POLICY_DATA:
        case RSVP_OBJ_ACCEPT_LABEL_SET:
        case RSVP_OBJ_PROTECTION:
        default:
            if (ndo->ndo_vflag <= 1)
                print_unknown_data(ndo, obj_tptr, "\n\t    ", obj_tlen); /* FIXME indentation */
            break;
        }
        /* do we also want to see a hex dump ? */
        if (ndo->ndo_vflag > 1 || hexdump == TRUE)
            print_unknown_data(ndo, tptr + sizeof(struct rsvp_object_header), "\n\t    ", /* FIXME indentation */
                               rsvp_obj_len - sizeof(struct rsvp_object_header));

        tptr+=rsvp_obj_len;
        tlen-=rsvp_obj_len;
    }
    return 0;
invalid:
    ND_PRINT((ndo, "%s", istr));
    return -1;
trunc:
    ND_PRINT((ndo, "\n\t\t"));
    ND_PRINT((ndo, "%s", tstr));
    return -1;
}
