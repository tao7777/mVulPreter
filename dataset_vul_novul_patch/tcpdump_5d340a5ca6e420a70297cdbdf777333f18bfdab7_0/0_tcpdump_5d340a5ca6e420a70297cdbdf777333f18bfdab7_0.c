cfm_print(netdissect_options *ndo,
          register const u_char *pptr, register u_int length)
{
    const struct cfm_common_header_t *cfm_common_header;
    const struct cfm_tlv_header_t *cfm_tlv_header;
    const uint8_t *tptr, *tlv_ptr;
    const uint8_t *namesp;
    u_int names_data_remaining;
    uint8_t md_nameformat, md_namelength;
    const uint8_t *md_name;
    uint8_t ma_nameformat, ma_namelength;
    const uint8_t *ma_name;
    u_int hexdump, tlen, cfm_tlv_len, cfm_tlv_type, ccm_interval;


    union {
        const struct cfm_ccm_t *cfm_ccm;
        const struct cfm_lbm_t *cfm_lbm;
        const struct cfm_ltm_t *cfm_ltm;
        const struct cfm_ltr_t *cfm_ltr;
    } msg_ptr;

    tptr=pptr;
    cfm_common_header = (const struct cfm_common_header_t *)pptr;
    if (length < sizeof(*cfm_common_header))
        goto tooshort;
    ND_TCHECK(*cfm_common_header);

    /*
     * Sanity checking of the header.
     */
    if (CFM_EXTRACT_VERSION(cfm_common_header->mdlevel_version) != CFM_VERSION) {
	ND_PRINT((ndo, "CFMv%u not supported, length %u",
               CFM_EXTRACT_VERSION(cfm_common_header->mdlevel_version), length));
	return;
    }

    ND_PRINT((ndo, "CFMv%u %s, MD Level %u, length %u",
           CFM_EXTRACT_VERSION(cfm_common_header->mdlevel_version),
           tok2str(cfm_opcode_values, "unknown (%u)", cfm_common_header->opcode),
           CFM_EXTRACT_MD_LEVEL(cfm_common_header->mdlevel_version),
           length));

    /*
     * In non-verbose mode just print the opcode and md-level.
     */
    if (ndo->ndo_vflag < 1) {
        return;
    }

    ND_PRINT((ndo, "\n\tFirst TLV offset %u", cfm_common_header->first_tlv_offset));

    tptr += sizeof(const struct cfm_common_header_t);
    tlen = length - sizeof(struct cfm_common_header_t);

    /*
     * Sanity check the first TLV offset.
     */
    if (cfm_common_header->first_tlv_offset > tlen) {
        ND_PRINT((ndo, " (too large, must be <= %u)", tlen));
        return;
    }

    switch (cfm_common_header->opcode) {
    case CFM_OPCODE_CCM:
        msg_ptr.cfm_ccm = (const struct cfm_ccm_t *)tptr;
        if (cfm_common_header->first_tlv_offset < sizeof(*msg_ptr.cfm_ccm)) {
            ND_PRINT((ndo, " (too small 1, must be >= %lu)",
                     (unsigned long) sizeof(*msg_ptr.cfm_ccm)));
            return;
        }
        if (tlen < sizeof(*msg_ptr.cfm_ccm))
            goto tooshort;
        ND_TCHECK(*msg_ptr.cfm_ccm);

        ccm_interval = CFM_EXTRACT_CCM_INTERVAL(cfm_common_header->flags);
        ND_PRINT((ndo, ", Flags [CCM Interval %u%s]",
               ccm_interval,
               cfm_common_header->flags & CFM_CCM_RDI_FLAG ?
               ", RDI" : ""));

        /*
         * Resolve the CCM interval field.
         */
        if (ccm_interval) {
            ND_PRINT((ndo, "\n\t  CCM Interval %.3fs"
                   ", min CCM Lifetime %.3fs, max CCM Lifetime %.3fs",
                   ccm_interval_base[ccm_interval],
                   ccm_interval_base[ccm_interval] * CCM_INTERVAL_MIN_MULTIPLIER,
                   ccm_interval_base[ccm_interval] * CCM_INTERVAL_MAX_MULTIPLIER));
        }

        ND_PRINT((ndo, "\n\t  Sequence Number 0x%08x, MA-End-Point-ID 0x%04x",
               EXTRACT_32BITS(msg_ptr.cfm_ccm->sequence),
               EXTRACT_16BITS(msg_ptr.cfm_ccm->ma_epi)));

        namesp = msg_ptr.cfm_ccm->names;
        names_data_remaining = sizeof(msg_ptr.cfm_ccm->names);

        /*
         * Resolve the MD fields.
         */
        md_nameformat = *namesp;
        namesp++;
        names_data_remaining--;  /* We know this is != 0 */
        if (md_nameformat != CFM_CCM_MD_FORMAT_NONE) {
            md_namelength = *namesp;
            namesp++;
            names_data_remaining--; /* We know this is !=0 */
            ND_PRINT((ndo, "\n\t  MD Name Format %s (%u), MD Name length %u",
                   tok2str(cfm_md_nameformat_values, "Unknown",
                           md_nameformat),
                   md_nameformat,
                   md_namelength));

            /*
             * -3 for the MA short name format and length and one byte
             * of MA short name.
             */
            if (md_namelength > names_data_remaining - 3) {
                ND_PRINT((ndo, " (too large, must be <= %u)", names_data_remaining - 2));
                return;
            }

            md_name = namesp;
            ND_PRINT((ndo, "\n\t  MD Name: "));
            switch (md_nameformat) {
            case CFM_CCM_MD_FORMAT_DNS:
            case CFM_CCM_MD_FORMAT_CHAR:
                safeputs(ndo, md_name, md_namelength);
                break;

            case CFM_CCM_MD_FORMAT_MAC:
                if (md_namelength == 6) {
                    ND_PRINT((ndo, "\n\t  MAC %s", etheraddr_string(ndo,
                               md_name)));
                } else {
                    ND_PRINT((ndo, "\n\t  MAC (length invalid)"));
                }
                break;

                /* FIXME add printers for those MD formats - hexdump for now */
            case CFM_CCM_MA_FORMAT_8021:
            default:
                print_unknown_data(ndo, md_name, "\n\t    ",
                                   md_namelength);
            }
            namesp += md_namelength;
            names_data_remaining -= md_namelength;
        } else {
            ND_PRINT((ndo, "\n\t  MD Name Format %s (%u)",
                   tok2str(cfm_md_nameformat_values, "Unknown",
                           md_nameformat),
                   md_nameformat));
        }


        /*
         * Resolve the MA fields.
         */
        ma_nameformat = *namesp;
        namesp++;
        names_data_remaining--; /* We know this is != 0 */
        ma_namelength = *namesp;
        namesp++;
        names_data_remaining--; /* We know this is != 0 */
        ND_PRINT((ndo, "\n\t  MA Name-Format %s (%u), MA name length %u",
               tok2str(cfm_ma_nameformat_values, "Unknown",
                       ma_nameformat),
               ma_nameformat,
               ma_namelength));

        if (ma_namelength > names_data_remaining) {
            ND_PRINT((ndo, " (too large, must be <= %u)", names_data_remaining));
            return;
        }

        ma_name = namesp;
        ND_PRINT((ndo, "\n\t  MA Name: "));
        switch (ma_nameformat) {
        case CFM_CCM_MA_FORMAT_CHAR:
            safeputs(ndo, ma_name, ma_namelength);
            break;

            /* FIXME add printers for those MA formats - hexdump for now */
        case CFM_CCM_MA_FORMAT_8021:
        case CFM_CCM_MA_FORMAT_VID:
        case CFM_CCM_MA_FORMAT_INT:
        case CFM_CCM_MA_FORMAT_VPN:
        default:
            print_unknown_data(ndo, ma_name, "\n\t    ", ma_namelength);
        }
        break;

    case CFM_OPCODE_LTM:
        msg_ptr.cfm_ltm = (const struct cfm_ltm_t *)tptr;
        if (cfm_common_header->first_tlv_offset < sizeof(*msg_ptr.cfm_ltm)) {
            ND_PRINT((ndo, " (too small 4, must be >= %lu)",
                     (unsigned long) sizeof(*msg_ptr.cfm_ltm)));
            return;
        }
        if (tlen < sizeof(*msg_ptr.cfm_ltm))
            goto tooshort;
        ND_TCHECK(*msg_ptr.cfm_ltm);

        ND_PRINT((ndo, ", Flags [%s]",
               bittok2str(cfm_ltm_flag_values, "none", cfm_common_header->flags)));

        ND_PRINT((ndo, "\n\t  Transaction-ID 0x%08x, ttl %u",
               EXTRACT_32BITS(msg_ptr.cfm_ltm->transaction_id),
               msg_ptr.cfm_ltm->ttl));

        ND_PRINT((ndo, "\n\t  Original-MAC %s, Target-MAC %s",
               etheraddr_string(ndo, msg_ptr.cfm_ltm->original_mac),
               etheraddr_string(ndo, msg_ptr.cfm_ltm->target_mac)));
        break;

    case CFM_OPCODE_LTR:
        msg_ptr.cfm_ltr = (const struct cfm_ltr_t *)tptr;
        if (cfm_common_header->first_tlv_offset < sizeof(*msg_ptr.cfm_ltr)) {
            ND_PRINT((ndo, " (too small 5, must be >= %lu)",
                     (unsigned long) sizeof(*msg_ptr.cfm_ltr)));
            return;
        }
        if (tlen < sizeof(*msg_ptr.cfm_ltr))
            goto tooshort;
        ND_TCHECK(*msg_ptr.cfm_ltr);

        ND_PRINT((ndo, ", Flags [%s]",
               bittok2str(cfm_ltr_flag_values, "none", cfm_common_header->flags)));

        ND_PRINT((ndo, "\n\t  Transaction-ID 0x%08x, ttl %u",
               EXTRACT_32BITS(msg_ptr.cfm_ltr->transaction_id),
               msg_ptr.cfm_ltr->ttl));

        ND_PRINT((ndo, "\n\t  Replay-Action %s (%u)",
               tok2str(cfm_ltr_replay_action_values,
                       "Unknown",
                       msg_ptr.cfm_ltr->replay_action),
               msg_ptr.cfm_ltr->replay_action));
        break;

        /*
         * No message decoder yet.
         * Hexdump everything up until the start of the TLVs
         */
    case CFM_OPCODE_LBR:
    case CFM_OPCODE_LBM:
    default:
        print_unknown_data(ndo, tptr, "\n\t  ",
                           tlen -  cfm_common_header->first_tlv_offset);
        break;
    }

    tptr += cfm_common_header->first_tlv_offset;
    tlen -= cfm_common_header->first_tlv_offset;

    while (tlen > 0) {
        cfm_tlv_header = (const struct cfm_tlv_header_t *)tptr;

        /* Enough to read the tlv type ? */
        ND_TCHECK2(*tptr, 1);
        cfm_tlv_type=cfm_tlv_header->type;

        ND_PRINT((ndo, "\n\t%s TLV (0x%02x)",
               tok2str(cfm_tlv_values, "Unknown", cfm_tlv_type),
               cfm_tlv_type));

        if (cfm_tlv_type == CFM_TLV_END) {
            /* Length is "Not present if the Type field is 0." */
            return;
        }

        /* do we have the full tlv header ? */
        if (tlen < sizeof(struct cfm_tlv_header_t))
            goto tooshort;
        ND_TCHECK2(*tptr, sizeof(struct cfm_tlv_header_t));
        cfm_tlv_len=EXTRACT_16BITS(&cfm_tlv_header->length);

        ND_PRINT((ndo, ", length %u", cfm_tlv_len));

        tptr += sizeof(struct cfm_tlv_header_t);
        tlen -= sizeof(struct cfm_tlv_header_t);
        tlv_ptr = tptr;

        /* do we have the full tlv ? */
        if (tlen < cfm_tlv_len)
            goto tooshort;
        ND_TCHECK2(*tptr, cfm_tlv_len);
        hexdump = FALSE;

        switch(cfm_tlv_type) {
        case CFM_TLV_PORT_STATUS:
            if (cfm_tlv_len < 1) {
                ND_PRINT((ndo, " (too short, must be >= 1)"));
                return;
            }
            ND_PRINT((ndo, ", Status: %s (%u)",
                   tok2str(cfm_tlv_port_status_values, "Unknown", *tptr),
                   *tptr));
            break;

        case CFM_TLV_INTERFACE_STATUS:
            if (cfm_tlv_len < 1) {
                ND_PRINT((ndo, " (too short, must be >= 1)"));
                return;
            }
            ND_PRINT((ndo, ", Status: %s (%u)",
                   tok2str(cfm_tlv_interface_status_values, "Unknown", *tptr),
                   *tptr));
            break;

        case CFM_TLV_PRIVATE:
            if (cfm_tlv_len < 4) {
                ND_PRINT((ndo, " (too short, must be >= 4)"));
                return;
            }
            ND_PRINT((ndo, ", Vendor: %s (%u), Sub-Type %u",
                   tok2str(oui_values,"Unknown", EXTRACT_24BITS(tptr)),
                   EXTRACT_24BITS(tptr),
                   *(tptr + 3)));
            hexdump = TRUE;
            break;

        case CFM_TLV_SENDER_ID:
        {
            u_int chassis_id_type, chassis_id_length;
            u_int mgmt_addr_length;
 
             if (cfm_tlv_len < 1) {
                 ND_PRINT((ndo, " (too short, must be >= 1)"));
                goto next_tlv;
             }
 
             /*
              * Get the Chassis ID length and check it.
             * IEEE 802.1Q-2014 Section 21.5.3.1
              */
             chassis_id_length = *tptr;
             tptr++;
             tlen--;
             cfm_tlv_len--;
 
             if (chassis_id_length) {
                /*
                 * IEEE 802.1Q-2014 Section 21.5.3.2: Chassis ID Subtype, references
                 * IEEE 802.1AB-2005 Section 9.5.2.2, subsequently
                 * IEEE 802.1AB-2016 Section 8.5.2.2: chassis ID subtype
                 */
                 if (cfm_tlv_len < 1) {
                     ND_PRINT((ndo, "\n\t  (TLV too short)"));
                    goto next_tlv;
                 }
                 chassis_id_type = *tptr;
                 cfm_tlv_len--;
                ND_PRINT((ndo, "\n\t  Chassis-ID Type %s (%u), Chassis-ID length %u",
                       tok2str(cfm_tlv_senderid_chassisid_values,
                               "Unknown",
                               chassis_id_type),
                       chassis_id_type,
                       chassis_id_length));
 
                 if (cfm_tlv_len < chassis_id_length) {
                     ND_PRINT((ndo, "\n\t  (TLV too short)"));
                    goto next_tlv;
                 }
 
                /* IEEE 802.1Q-2014 Section 21.5.3.3: Chassis ID */
                 switch (chassis_id_type) {
                 case CFM_CHASSIS_ID_MAC_ADDRESS:
                    if (chassis_id_length != ETHER_ADDR_LEN) {
                        ND_PRINT((ndo, " (invalid MAC address length)"));
                        hexdump = TRUE;
                        break;
                    }
                     ND_PRINT((ndo, "\n\t  MAC %s", etheraddr_string(ndo, tptr + 1)));
                     break;
 
                 case CFM_CHASSIS_ID_NETWORK_ADDRESS:
                    hexdump |= cfm_network_addr_print(ndo, tptr + 1, chassis_id_length);
                     break;
 
                 case CFM_CHASSIS_ID_INTERFACE_NAME: /* fall through */
                case CFM_CHASSIS_ID_INTERFACE_ALIAS:
                case CFM_CHASSIS_ID_LOCAL:
                case CFM_CHASSIS_ID_CHASSIS_COMPONENT:
                case CFM_CHASSIS_ID_PORT_COMPONENT:
                    safeputs(ndo, tptr + 1, chassis_id_length);
                    break;

                default:
                    hexdump = TRUE;
                    break;
                }
                cfm_tlv_len -= chassis_id_length;

                tptr += 1 + chassis_id_length;
                tlen -= 1 + chassis_id_length;
            }
 
             /*
              * Check if there is a Management Address.
             * IEEE 802.1Q-2014 Section 21.5.3.4: Management Address Domain Length
             * This and all subsequent fields are not present if the TLV length
             * allows only the above fields.
              */
             if (cfm_tlv_len == 0) {
                 /* No, there isn't; we're done. */
                break;
             }
 
            /* Here mgmt_addr_length stands for the management domain length. */
             mgmt_addr_length = *tptr;
             tptr++;
             tlen--;
             cfm_tlv_len--;
            ND_PRINT((ndo, "\n\t  Management Address Domain Length %u", mgmt_addr_length));
             if (mgmt_addr_length) {
                /* IEEE 802.1Q-2014 Section 21.5.3.5: Management Address Domain */
                 if (cfm_tlv_len < mgmt_addr_length) {
                     ND_PRINT((ndo, "\n\t  (TLV too short)"));
                    goto next_tlv;
                 }
                 cfm_tlv_len -= mgmt_addr_length;
                 /*
                  * XXX - this is an OID; print it as such.
                  */
                hex_print(ndo, "\n\t  Management Address Domain: ", tptr, mgmt_addr_length);
                 tptr += mgmt_addr_length;
                 tlen -= mgmt_addr_length;
 
                /*
                 * IEEE 802.1Q-2014 Section 21.5.3.6: Management Address Length
                 * This field is present if Management Address Domain Length is not 0.
                 */
                 if (cfm_tlv_len < 1) {
                    ND_PRINT((ndo, " (Management Address Length is missing)"));
                    hexdump = TRUE;
                    break;
                 }
 
                /* Here mgmt_addr_length stands for the management address length. */
                 mgmt_addr_length = *tptr;
                 tptr++;
                 tlen--;
                 cfm_tlv_len--;
                ND_PRINT((ndo, "\n\t  Management Address Length %u", mgmt_addr_length));
                 if (mgmt_addr_length) {
                    /* IEEE 802.1Q-2014 Section 21.5.3.7: Management Address */
                     if (cfm_tlv_len < mgmt_addr_length) {
                         ND_PRINT((ndo, "\n\t  (TLV too short)"));
                         return;
                    }
                    cfm_tlv_len -= mgmt_addr_length;
                     /*
                      * XXX - this is a TransportDomain; print it as such.
                      */
                    hex_print(ndo, "\n\t  Management Address: ", tptr, mgmt_addr_length);
                     tptr += mgmt_addr_length;
                     tlen -= mgmt_addr_length;
                 }
            }
            break;
        }

            /*
             * FIXME those are the defined TLVs that lack a decoder
             * you are welcome to contribute code ;-)
             */

        case CFM_TLV_DATA:
        case CFM_TLV_REPLY_INGRESS:
        case CFM_TLV_REPLY_EGRESS:
        default:
            hexdump = TRUE;
            break;
        }
        /* do we want to see an additional hexdump ? */
         if (hexdump || ndo->ndo_vflag > 1)
             print_unknown_data(ndo, tlv_ptr, "\n\t  ", cfm_tlv_len);
 
next_tlv:
         tptr+=cfm_tlv_len;
         tlen-=cfm_tlv_len;
     }
    return;

tooshort:
    ND_PRINT((ndo, "\n\t\t packet is too short"));
    return;

trunc:
    ND_PRINT((ndo, "\n\t\t packet exceeded snapshot"));
}
