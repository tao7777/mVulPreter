lmp_print(netdissect_options *ndo,
          register const u_char *pptr, register u_int len)
{
    const struct lmp_common_header *lmp_com_header;
    const struct lmp_object_header *lmp_obj_header;
    const u_char *tptr,*obj_tptr;
    u_int tlen,lmp_obj_len,lmp_obj_ctype,obj_tlen;
    int hexdump, ret;
    u_int offset;
    u_int link_type;

    union { /* int to float conversion buffer */
        float f;
        uint32_t i;
    } bw;

    tptr=pptr;
    lmp_com_header = (const struct lmp_common_header *)pptr;
    ND_TCHECK(*lmp_com_header);

    /*
     * Sanity checking of the header.
     */
    if (LMP_EXTRACT_VERSION(lmp_com_header->version_res[0]) != LMP_VERSION) {
	ND_PRINT((ndo, "LMP version %u packet not supported",
               LMP_EXTRACT_VERSION(lmp_com_header->version_res[0])));
	return;
    }

    /* in non-verbose mode just lets print the basic Message Type*/
    if (ndo->ndo_vflag < 1) {
        ND_PRINT((ndo, "LMPv%u %s Message, length: %u",
               LMP_EXTRACT_VERSION(lmp_com_header->version_res[0]),
               tok2str(lmp_msg_type_values, "unknown (%u)",lmp_com_header->msg_type),
               len));
        return;
    }

    /* ok they seem to want to know everything - lets fully decode it */

    tlen=EXTRACT_16BITS(lmp_com_header->length);

    ND_PRINT((ndo, "\n\tLMPv%u, msg-type: %s, Flags: [%s], length: %u",
           LMP_EXTRACT_VERSION(lmp_com_header->version_res[0]),
           tok2str(lmp_msg_type_values, "unknown, type: %u",lmp_com_header->msg_type),
           bittok2str(lmp_header_flag_values,"none",lmp_com_header->flags),
           tlen));
    if (tlen < sizeof(const struct lmp_common_header)) {
        ND_PRINT((ndo, " (too short)"));
        return;
    }
    if (tlen > len) {
        ND_PRINT((ndo, " (too long)"));
        tlen = len;
    }

    tptr+=sizeof(const struct lmp_common_header);
    tlen-=sizeof(const struct lmp_common_header);

    while(tlen>0) {
        /* did we capture enough for fully decoding the object header ? */
        ND_TCHECK2(*tptr, sizeof(struct lmp_object_header));

        lmp_obj_header = (const struct lmp_object_header *)tptr;
        lmp_obj_len=EXTRACT_16BITS(lmp_obj_header->length);
        lmp_obj_ctype=(lmp_obj_header->ctype)&0x7f;

        ND_PRINT((ndo, "\n\t  %s Object (%u), Class-Type: %s (%u) Flags: [%snegotiable], length: %u",
               tok2str(lmp_obj_values,
                       "Unknown",
                       lmp_obj_header->class_num),
               lmp_obj_header->class_num,
               tok2str(lmp_ctype_values,
                       "Unknown",
                       ((lmp_obj_header->class_num)<<8)+lmp_obj_ctype),
               lmp_obj_ctype,
               (lmp_obj_header->ctype)&0x80 ? "" : "non-",
               lmp_obj_len));

        if (lmp_obj_len < 4) {
            ND_PRINT((ndo, " (too short)"));
            return;
        }
        if ((lmp_obj_len % 4) != 0) {
            ND_PRINT((ndo, " (not a multiple of 4)"));
            return;
        }

        obj_tptr=tptr+sizeof(struct lmp_object_header);
        obj_tlen=lmp_obj_len-sizeof(struct lmp_object_header);

        /* did we capture enough for fully decoding the object ? */
        ND_TCHECK2(*tptr, lmp_obj_len);
        hexdump=FALSE;

        switch(lmp_obj_header->class_num) {

        case LMP_OBJ_CC_ID:
            switch(lmp_obj_ctype) {
            case LMP_CTYPE_LOC:
            case LMP_CTYPE_RMT:
                if (obj_tlen != 4) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
                ND_PRINT((ndo, "\n\t    Control Channel ID: %u (0x%08x)",
                       EXTRACT_32BITS(obj_tptr),
                       EXTRACT_32BITS(obj_tptr)));
                break;

            default:
                hexdump=TRUE;
            }
            break;

        case LMP_OBJ_LINK_ID:
        case LMP_OBJ_INTERFACE_ID:
            switch(lmp_obj_ctype) {
            case LMP_CTYPE_IPV4_LOC:
            case LMP_CTYPE_IPV4_RMT:
                if (obj_tlen != 4) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
                ND_PRINT((ndo, "\n\t    IPv4 Link ID: %s (0x%08x)",
                       ipaddr_string(ndo, obj_tptr),
                       EXTRACT_32BITS(obj_tptr)));
                break;
            case LMP_CTYPE_IPV6_LOC:
            case LMP_CTYPE_IPV6_RMT:
                if (obj_tlen != 16) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
                ND_PRINT((ndo, "\n\t    IPv6 Link ID: %s (0x%08x)",
                       ip6addr_string(ndo, obj_tptr),
                       EXTRACT_32BITS(obj_tptr)));
                break;
            case LMP_CTYPE_UNMD_LOC:
            case LMP_CTYPE_UNMD_RMT:
                if (obj_tlen != 4) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
                ND_PRINT((ndo, "\n\t    Link ID: %u (0x%08x)",
                       EXTRACT_32BITS(obj_tptr),
                       EXTRACT_32BITS(obj_tptr)));
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case LMP_OBJ_MESSAGE_ID:
            switch(lmp_obj_ctype) {
            case LMP_CTYPE_1:
                if (obj_tlen != 4) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
                ND_PRINT((ndo, "\n\t    Message ID: %u (0x%08x)",
                       EXTRACT_32BITS(obj_tptr),
                       EXTRACT_32BITS(obj_tptr)));
                break;
            case LMP_CTYPE_2:
                if (obj_tlen != 4) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
                ND_PRINT((ndo, "\n\t    Message ID Ack: %u (0x%08x)",
                       EXTRACT_32BITS(obj_tptr),
                       EXTRACT_32BITS(obj_tptr)));
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case LMP_OBJ_NODE_ID:
            switch(lmp_obj_ctype) {
            case LMP_CTYPE_LOC:
            case LMP_CTYPE_RMT:
                if (obj_tlen != 4) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
                ND_PRINT((ndo, "\n\t    Node ID: %s (0x%08x)",
                       ipaddr_string(ndo, obj_tptr),
                       EXTRACT_32BITS(obj_tptr)));
                break;

            default:
                hexdump=TRUE;
            }
            break;

        case LMP_OBJ_CONFIG:
            switch(lmp_obj_ctype) {
            case LMP_CTYPE_HELLO_CONFIG:
                if (obj_tlen != 4) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
                ND_PRINT((ndo, "\n\t    Hello Interval: %u\n\t    Hello Dead Interval: %u",
                       EXTRACT_16BITS(obj_tptr),
                       EXTRACT_16BITS(obj_tptr+2)));
                break;

            default:
                hexdump=TRUE;
            }
            break;

        case LMP_OBJ_HELLO:
            switch(lmp_obj_ctype) {
	    case LMP_CTYPE_HELLO:
                if (obj_tlen != 8) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
                ND_PRINT((ndo, "\n\t    Tx Seq: %u, Rx Seq: %u",
                       EXTRACT_32BITS(obj_tptr),
                       EXTRACT_32BITS(obj_tptr+4)));
                break;

            default:
                hexdump=TRUE;
            }
            break;

        case LMP_OBJ_TE_LINK:
	    switch(lmp_obj_ctype) {
	    case LMP_CTYPE_IPV4:
                if (obj_tlen != 12) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
		ND_PRINT((ndo, "\n\t    Flags: [%s]",
		    bittok2str(lmp_obj_te_link_flag_values,
			"none",
			EXTRACT_8BITS(obj_tptr))));

		ND_PRINT((ndo, "\n\t    Local Link-ID: %s (0x%08x)"
		       "\n\t    Remote Link-ID: %s (0x%08x)",
                       ipaddr_string(ndo, obj_tptr+4),
                       EXTRACT_32BITS(obj_tptr+4),
                       ipaddr_string(ndo, obj_tptr+8),
                       EXTRACT_32BITS(obj_tptr+8)));
		break;

	    case LMP_CTYPE_IPV6:
                if (obj_tlen != 36) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
		ND_PRINT((ndo, "\n\t    Flags: [%s]",
		    bittok2str(lmp_obj_te_link_flag_values,
			"none",
			EXTRACT_8BITS(obj_tptr))));

		ND_PRINT((ndo, "\n\t    Local Link-ID: %s (0x%08x)"
		       "\n\t    Remote Link-ID: %s (0x%08x)",
                       ip6addr_string(ndo, obj_tptr+4),
                       EXTRACT_32BITS(obj_tptr+4),
                       ip6addr_string(ndo, obj_tptr+20),
                       EXTRACT_32BITS(obj_tptr+20)));
                break;

	    case LMP_CTYPE_UNMD:
                if (obj_tlen != 12) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
		ND_PRINT((ndo, "\n\t    Flags: [%s]",
		    bittok2str(lmp_obj_te_link_flag_values,
			"none",
			EXTRACT_8BITS(obj_tptr))));

		ND_PRINT((ndo, "\n\t    Local Link-ID: %u (0x%08x)"
		       "\n\t    Remote Link-ID: %u (0x%08x)",
                       EXTRACT_32BITS(obj_tptr+4),
                       EXTRACT_32BITS(obj_tptr+4),
                       EXTRACT_32BITS(obj_tptr+8),
                       EXTRACT_32BITS(obj_tptr+8)));
		break;

            default:
                hexdump=TRUE;
            }
            break;

        case LMP_OBJ_DATA_LINK:
	    switch(lmp_obj_ctype) {
	    case LMP_CTYPE_IPV4:
                if (obj_tlen < 12) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
	        ND_PRINT((ndo, "\n\t    Flags: [%s]",
		    bittok2str(lmp_obj_data_link_flag_values,
			"none",
			EXTRACT_8BITS(obj_tptr))));
                ND_PRINT((ndo, "\n\t    Local Interface ID: %s (0x%08x)"
                       "\n\t    Remote Interface ID: %s (0x%08x)",
                       ipaddr_string(ndo, obj_tptr+4),
                       EXTRACT_32BITS(obj_tptr+4),
                       ipaddr_string(ndo, obj_tptr+8),
                       EXTRACT_32BITS(obj_tptr+8)));

		ret = lmp_print_data_link_subobjs(ndo, obj_tptr, obj_tlen - 12, 12);
		if (ret == -1)
		    goto trunc;
		if (ret == TRUE)
		    hexdump=TRUE;
		break;

	    case LMP_CTYPE_IPV6:
                if (obj_tlen < 36) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
	        ND_PRINT((ndo, "\n\t    Flags: [%s]",
		    bittok2str(lmp_obj_data_link_flag_values,
			"none",
			EXTRACT_8BITS(obj_tptr))));
                ND_PRINT((ndo, "\n\t    Local Interface ID: %s (0x%08x)"
                       "\n\t    Remote Interface ID: %s (0x%08x)",
                       ip6addr_string(ndo, obj_tptr+4),
                       EXTRACT_32BITS(obj_tptr+4),
                       ip6addr_string(ndo, obj_tptr+20),
                       EXTRACT_32BITS(obj_tptr+20)));

		ret = lmp_print_data_link_subobjs(ndo, obj_tptr, obj_tlen - 36, 36);
		if (ret == -1)
		    goto trunc;
		if (ret == TRUE)
		    hexdump=TRUE;
		break;

	    case LMP_CTYPE_UNMD:
                if (obj_tlen < 12) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
	        ND_PRINT((ndo, "\n\t    Flags: [%s]",
		    bittok2str(lmp_obj_data_link_flag_values,
			"none",
			EXTRACT_8BITS(obj_tptr))));
                ND_PRINT((ndo, "\n\t    Local Interface ID: %u (0x%08x)"
                       "\n\t    Remote Interface ID: %u (0x%08x)",
                       EXTRACT_32BITS(obj_tptr+4),
                       EXTRACT_32BITS(obj_tptr+4),
                       EXTRACT_32BITS(obj_tptr+8),
                       EXTRACT_32BITS(obj_tptr+8)));

		ret = lmp_print_data_link_subobjs(ndo, obj_tptr, obj_tlen - 12, 12);
		if (ret == -1)
		    goto trunc;
		if (ret == TRUE)
		    hexdump=TRUE;
		break;

            default:
                hexdump=TRUE;
            }
            break;

        case LMP_OBJ_VERIFY_BEGIN:
	    switch(lmp_obj_ctype) {
            case LMP_CTYPE_1:
                if (obj_tlen != 20) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
		ND_PRINT((ndo, "\n\t    Flags: %s",
		bittok2str(lmp_obj_begin_verify_flag_values,
			"none",
			EXTRACT_16BITS(obj_tptr))));
		ND_PRINT((ndo, "\n\t    Verify Interval: %u",
			EXTRACT_16BITS(obj_tptr+2)));
		ND_PRINT((ndo, "\n\t    Data links: %u",
			EXTRACT_32BITS(obj_tptr+4)));
                ND_PRINT((ndo, "\n\t    Encoding type: %s",
			tok2str(gmpls_encoding_values, "Unknown", *(obj_tptr+8))));
                ND_PRINT((ndo, "\n\t    Verify Transport Mechanism: %u (0x%x)%s",
			EXTRACT_16BITS(obj_tptr+10),
			EXTRACT_16BITS(obj_tptr+10),
			EXTRACT_16BITS(obj_tptr+10)&8000 ? " (Payload test messages capable)" : ""));
                bw.i = EXTRACT_32BITS(obj_tptr+12);
		ND_PRINT((ndo, "\n\t    Transmission Rate: %.3f Mbps",bw.f*8/1000000));
		ND_PRINT((ndo, "\n\t    Wavelength: %u",
			EXTRACT_32BITS(obj_tptr+16)));
		break;

            default:
                hexdump=TRUE;
            }
            break;

        case LMP_OBJ_VERIFY_BEGIN_ACK:
	    switch(lmp_obj_ctype) {
            case LMP_CTYPE_1:
                if (obj_tlen != 4) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
                ND_PRINT((ndo, "\n\t    Verify Dead Interval: %u"
                       "\n\t    Verify Transport Response: %u",
                       EXTRACT_16BITS(obj_tptr),
                       EXTRACT_16BITS(obj_tptr+2)));
                break;

            default:
                hexdump=TRUE;
            }
            break;

	case LMP_OBJ_VERIFY_ID:
	    switch(lmp_obj_ctype) {
            case LMP_CTYPE_1:
                if (obj_tlen != 4) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
                ND_PRINT((ndo, "\n\t    Verify ID: %u",
                       EXTRACT_32BITS(obj_tptr)));
                break;

            default:
                hexdump=TRUE;
            }
            break;

	case LMP_OBJ_CHANNEL_STATUS:
            switch(lmp_obj_ctype) {
	    case LMP_CTYPE_IPV4:
		offset = 0;
		/* Decode pairs: <Interface_ID (4 bytes), Channel_status (4 bytes)> */
		while (offset+8 <= obj_tlen) {
			ND_PRINT((ndo, "\n\t    Interface ID: %s (0x%08x)",
			ipaddr_string(ndo, obj_tptr+offset),
			EXTRACT_32BITS(obj_tptr+offset)));

			ND_PRINT((ndo, "\n\t\t    Active: %s (%u)",
				(EXTRACT_32BITS(obj_tptr+offset+4)>>31) ?
						"Allocated" : "Non-allocated",
				(EXTRACT_32BITS(obj_tptr+offset+4)>>31)));

			ND_PRINT((ndo, "\n\t\t    Direction: %s (%u)",
				(EXTRACT_32BITS(obj_tptr+offset+4)>>30)&0x1 ?
						"Transmit" : "Receive",
				(EXTRACT_32BITS(obj_tptr+offset+4)>>30)&0x1));

			ND_PRINT((ndo, "\n\t\t    Channel Status: %s (%u)",
					tok2str(lmp_obj_channel_status_values,
			 		"Unknown",
					EXTRACT_32BITS(obj_tptr+offset+4)&0x3FFFFFF),
			EXTRACT_32BITS(obj_tptr+offset+4)&0x3FFFFFF));
			offset+=8;
		}
                break;

	    case LMP_CTYPE_IPV6:
		offset = 0;
		/* Decode pairs: <Interface_ID (16 bytes), Channel_status (4 bytes)> */
		while (offset+20 <= obj_tlen) {
			ND_PRINT((ndo, "\n\t    Interface ID: %s (0x%08x)",
			ip6addr_string(ndo, obj_tptr+offset),
			EXTRACT_32BITS(obj_tptr+offset)));

			ND_PRINT((ndo, "\n\t\t    Active: %s (%u)",
				(EXTRACT_32BITS(obj_tptr+offset+16)>>31) ?
						"Allocated" : "Non-allocated",
				(EXTRACT_32BITS(obj_tptr+offset+16)>>31)));

			ND_PRINT((ndo, "\n\t\t    Direction: %s (%u)",
				(EXTRACT_32BITS(obj_tptr+offset+16)>>30)&0x1 ?
						"Transmit" : "Receive",
				(EXTRACT_32BITS(obj_tptr+offset+16)>>30)&0x1));

			ND_PRINT((ndo, "\n\t\t    Channel Status: %s (%u)",
					tok2str(lmp_obj_channel_status_values,
					"Unknown",
					EXTRACT_32BITS(obj_tptr+offset+16)&0x3FFFFFF),
			EXTRACT_32BITS(obj_tptr+offset+16)&0x3FFFFFF));
			offset+=20;
		}
                break;

	    case LMP_CTYPE_UNMD:
		offset = 0;
		/* Decode pairs: <Interface_ID (4 bytes), Channel_status (4 bytes)> */
		while (offset+8 <= obj_tlen) {
			ND_PRINT((ndo, "\n\t    Interface ID: %u (0x%08x)",
			EXTRACT_32BITS(obj_tptr+offset),
			EXTRACT_32BITS(obj_tptr+offset)));

			ND_PRINT((ndo, "\n\t\t    Active: %s (%u)",
				(EXTRACT_32BITS(obj_tptr+offset+4)>>31) ?
						"Allocated" : "Non-allocated",
				(EXTRACT_32BITS(obj_tptr+offset+4)>>31)));

			ND_PRINT((ndo, "\n\t\t    Direction: %s (%u)",
				(EXTRACT_32BITS(obj_tptr+offset+4)>>30)&0x1 ?
						"Transmit" : "Receive",
				(EXTRACT_32BITS(obj_tptr+offset+4)>>30)&0x1));

			ND_PRINT((ndo, "\n\t\t    Channel Status: %s (%u)",
					tok2str(lmp_obj_channel_status_values,
					"Unknown",
					EXTRACT_32BITS(obj_tptr+offset+4)&0x3FFFFFF),
			EXTRACT_32BITS(obj_tptr+offset+4)&0x3FFFFFF));
			offset+=8;
		}
                break;

            default:
                hexdump=TRUE;
            }
            break;

	case LMP_OBJ_CHANNEL_STATUS_REQ:
            switch(lmp_obj_ctype) {
	    case LMP_CTYPE_IPV4:
		offset = 0;
		while (offset+4 <= obj_tlen) {
			ND_PRINT((ndo, "\n\t    Interface ID: %s (0x%08x)",
			ipaddr_string(ndo, obj_tptr+offset),
			EXTRACT_32BITS(obj_tptr+offset)));
			offset+=4;
		}
                break;

	    case LMP_CTYPE_IPV6:
		offset = 0;
		while (offset+16 <= obj_tlen) {
			ND_PRINT((ndo, "\n\t    Interface ID: %s (0x%08x)",
			ip6addr_string(ndo, obj_tptr+offset),
			EXTRACT_32BITS(obj_tptr+offset)));
			offset+=16;
		}
                break;

	    case LMP_CTYPE_UNMD:
		offset = 0;
		while (offset+4 <= obj_tlen) {
			ND_PRINT((ndo, "\n\t    Interface ID: %u (0x%08x)",
			EXTRACT_32BITS(obj_tptr+offset),
			EXTRACT_32BITS(obj_tptr+offset)));
			offset+=4;
		}
                break;

	    default:
                hexdump=TRUE;
            }
            break;

        case LMP_OBJ_ERROR_CODE:
	    switch(lmp_obj_ctype) {
            case LMP_CTYPE_BEGIN_VERIFY_ERROR:
                if (obj_tlen != 4) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
		ND_PRINT((ndo, "\n\t    Error Code: %s",
		bittok2str(lmp_obj_begin_verify_error_values,
			"none",
			EXTRACT_32BITS(obj_tptr))));
                break;

            case LMP_CTYPE_LINK_SUMMARY_ERROR:
                if (obj_tlen != 4) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
		ND_PRINT((ndo, "\n\t    Error Code: %s",
		bittok2str(lmp_obj_link_summary_error_values,
			"none",
			EXTRACT_32BITS(obj_tptr))));
                break;
            default:
                hexdump=TRUE;
            }
            break;

	case LMP_OBJ_SERVICE_CONFIG:
	    switch (lmp_obj_ctype) {
	    case LMP_CTYPE_SERVICE_CONFIG_SP:
                if (obj_tlen != 4) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }
		ND_PRINT((ndo, "\n\t Flags: %s",
		       bittok2str(lmp_obj_service_config_sp_flag_values,
				  "none",
				  EXTRACT_8BITS(obj_tptr))));

		ND_PRINT((ndo, "\n\t  UNI Version: %u",
		       EXTRACT_8BITS(obj_tptr + 1)));

		break;

            case LMP_CTYPE_SERVICE_CONFIG_CPSA:
                if (obj_tlen != 16) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }

		link_type = EXTRACT_8BITS(obj_tptr);

		ND_PRINT((ndo, "\n\t Link Type: %s (%u)",
		       tok2str(lmp_sd_service_config_cpsa_link_type_values,
			       "Unknown", link_type),
		       link_type));

		switch (link_type) {
		case LMP_SD_SERVICE_CONFIG_CPSA_LINK_TYPE_SDH:
		    ND_PRINT((ndo, "\n\t Signal Type: %s (%u)",
			   tok2str(lmp_sd_service_config_cpsa_signal_type_sdh_values,
				   "Unknown",
				   EXTRACT_8BITS(obj_tptr + 1)),
				   EXTRACT_8BITS(obj_tptr + 1)));
		    break;

		case LMP_SD_SERVICE_CONFIG_CPSA_LINK_TYPE_SONET:
		    ND_PRINT((ndo, "\n\t Signal Type: %s (%u)",
			   tok2str(lmp_sd_service_config_cpsa_signal_type_sonet_values,
				   "Unknown",
				   EXTRACT_8BITS(obj_tptr + 1)),
				   EXTRACT_8BITS(obj_tptr + 1)));
		    break;
		}

		ND_PRINT((ndo, "\n\t Transparency: %s",
		       bittok2str(lmp_obj_service_config_cpsa_tp_flag_values,
				  "none",
				  EXTRACT_8BITS(obj_tptr + 2))));

		ND_PRINT((ndo, "\n\t Contiguous Concatenation Types: %s",
		       bittok2str(lmp_obj_service_config_cpsa_cct_flag_values,
				  "none",
				  EXTRACT_8BITS(obj_tptr + 3))));

		ND_PRINT((ndo, "\n\t Minimum NCC: %u",
		       EXTRACT_16BITS(obj_tptr+4)));

		ND_PRINT((ndo, "\n\t Maximum NCC: %u",
		       EXTRACT_16BITS(obj_tptr+6)));

		ND_PRINT((ndo, "\n\t Minimum NVC:%u",
		       EXTRACT_16BITS(obj_tptr+8)));

		ND_PRINT((ndo, "\n\t Maximum NVC:%u",
		       EXTRACT_16BITS(obj_tptr+10)));

		ND_PRINT((ndo, "\n\t    Local Interface ID: %s (0x%08x)",
		       ipaddr_string(ndo, obj_tptr+12),
		       EXTRACT_32BITS(obj_tptr+12)));

		break;

	    case LMP_CTYPE_SERVICE_CONFIG_TRANSPARENCY_TCM:
                if (obj_tlen != 8) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }

		ND_PRINT((ndo, "\n\t Transparency Flags: %s",
		       bittok2str(
			   lmp_obj_service_config_nsa_transparency_flag_values,
			   "none",
			   EXTRACT_32BITS(obj_tptr))));

		ND_PRINT((ndo, "\n\t TCM Monitoring Flags: %s",
		       bittok2str(
			   lmp_obj_service_config_nsa_tcm_flag_values,
			   "none",
			   EXTRACT_8BITS(obj_tptr + 7))));

		break;

	    case LMP_CTYPE_SERVICE_CONFIG_NETWORK_DIVERSITY:
                if (obj_tlen != 4) {
                    ND_PRINT((ndo, " (not correct for object)"));
                    break;
                }

		ND_PRINT((ndo, "\n\t Diversity: Flags: %s",
		       bittok2str(
			   lmp_obj_service_config_nsa_network_diversity_flag_values,
			   "none",
			   EXTRACT_8BITS(obj_tptr + 3))));
		break;

	    default:
		hexdump = TRUE;
	    }

	break;

        default:
            if (ndo->ndo_vflag <= 1)
                print_unknown_data(ndo,obj_tptr,"\n\t    ",obj_tlen);
            break;
        }
        /* do we want to see an additionally hexdump ? */
        if (ndo->ndo_vflag > 1 || hexdump==TRUE)
            print_unknown_data(ndo,tptr+sizeof(struct lmp_object_header),"\n\t    ",
                               lmp_obj_len-sizeof(struct lmp_object_header));

        tptr+=lmp_obj_len;
        tlen-=lmp_obj_len;
     }
     return;
 trunc:
    ND_PRINT((ndo, "%s", tstr));
 }
