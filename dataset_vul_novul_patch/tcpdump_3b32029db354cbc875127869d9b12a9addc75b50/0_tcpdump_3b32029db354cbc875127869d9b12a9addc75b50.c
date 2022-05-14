isis_print(netdissect_options *ndo,
           const uint8_t *p, u_int length)
{
    const struct isis_common_header *isis_header;

    const struct isis_iih_lan_header *header_iih_lan;
    const struct isis_iih_ptp_header *header_iih_ptp;
    const struct isis_lsp_header *header_lsp;
    const struct isis_csnp_header *header_csnp;
    const struct isis_psnp_header *header_psnp;

    const struct isis_tlv_lsp *tlv_lsp;
    const struct isis_tlv_ptp_adj *tlv_ptp_adj;
    const struct isis_tlv_is_reach *tlv_is_reach;
    const struct isis_tlv_es_reach *tlv_es_reach;

    uint8_t pdu_type, max_area, id_length, tlv_type, tlv_len, tmp, alen, lan_alen, prefix_len;
    uint8_t ext_is_len, ext_ip_len, mt_len;
    const uint8_t *optr, *pptr, *tptr;
    u_short packet_len,pdu_len, key_id;
    u_int i,vendor_id;
    int sigcheck;

    packet_len=length;
    optr = p; /* initialize the _o_riginal pointer to the packet start -
                 need it for parsing the checksum TLV and authentication
                 TLV verification */
    isis_header = (const struct isis_common_header *)p;
    ND_TCHECK(*isis_header);
    if (length < ISIS_COMMON_HEADER_SIZE)
        goto trunc;
    pptr = p+(ISIS_COMMON_HEADER_SIZE);
    header_iih_lan = (const struct isis_iih_lan_header *)pptr;
    header_iih_ptp = (const struct isis_iih_ptp_header *)pptr;
    header_lsp = (const struct isis_lsp_header *)pptr;
    header_csnp = (const struct isis_csnp_header *)pptr;
    header_psnp = (const struct isis_psnp_header *)pptr;

    if (!ndo->ndo_eflag)
        ND_PRINT((ndo, "IS-IS"));

    /*
     * Sanity checking of the header.
     */

    if (isis_header->version != ISIS_VERSION) {
	ND_PRINT((ndo, "version %d packet not supported", isis_header->version));
	return (0);
    }

    if ((isis_header->id_length != SYSTEM_ID_LEN) && (isis_header->id_length != 0)) {
	ND_PRINT((ndo, "system ID length of %d is not supported",
	       isis_header->id_length));
	return (0);
    }

    if (isis_header->pdu_version != ISIS_VERSION) {
	ND_PRINT((ndo, "version %d packet not supported", isis_header->pdu_version));
	return (0);
    }

    if (length < isis_header->fixed_len) {
	ND_PRINT((ndo, "fixed header length %u > packet length %u", isis_header->fixed_len, length));
	return (0);
    }

    if (isis_header->fixed_len < ISIS_COMMON_HEADER_SIZE) {
	ND_PRINT((ndo, "fixed header length %u < minimum header size %u", isis_header->fixed_len, (u_int)ISIS_COMMON_HEADER_SIZE));
	return (0);
    }

    max_area = isis_header->max_area;
    switch(max_area) {
    case 0:
	max_area = 3;	 /* silly shit */
	break;
    case 255:
	ND_PRINT((ndo, "bad packet -- 255 areas"));
	return (0);
    default:
	break;
    }

    id_length = isis_header->id_length;
    switch(id_length) {
    case 0:
        id_length = 6;	 /* silly shit again */
	break;
    case 1:              /* 1-8 are valid sys-ID lenghts */
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        break;
    case 255:
        id_length = 0;   /* entirely useless */
	break;
    default:
        break;
    }

    /* toss any non 6-byte sys-ID len PDUs */
    if (id_length != 6 ) {
	ND_PRINT((ndo, "bad packet -- illegal sys-ID length (%u)", id_length));
	return (0);
    }

    pdu_type=isis_header->pdu_type;

    /* in non-verbose mode print the basic PDU Type plus PDU specific brief information*/
    if (ndo->ndo_vflag == 0) {
        ND_PRINT((ndo, "%s%s",
               ndo->ndo_eflag ? "" : ", ",
               tok2str(isis_pdu_values, "unknown PDU-Type %u", pdu_type)));
    } else {
        /* ok they seem to want to know everything - lets fully decode it */
        ND_PRINT((ndo, "%slength %u", ndo->ndo_eflag ? "" : ", ", length));

        ND_PRINT((ndo, "\n\t%s, hlen: %u, v: %u, pdu-v: %u, sys-id-len: %u (%u), max-area: %u (%u)",
               tok2str(isis_pdu_values,
                       "unknown, type %u",
                       pdu_type),
               isis_header->fixed_len,
               isis_header->version,
               isis_header->pdu_version,
               id_length,
               isis_header->id_length,
               max_area,
               isis_header->max_area));

        if (ndo->ndo_vflag > 1) {
            if (!print_unknown_data(ndo, optr, "\n\t", 8)) /* provide the _o_riginal pointer */
                return (0);                         /* for optionally debugging the common header */
        }
    }

    switch (pdu_type) {

    case ISIS_PDU_L1_LAN_IIH:
    case ISIS_PDU_L2_LAN_IIH:
        if (isis_header->fixed_len != (ISIS_COMMON_HEADER_SIZE+ISIS_IIH_LAN_HEADER_SIZE)) {
            ND_PRINT((ndo, ", bogus fixed header length %u should be %lu",
                     isis_header->fixed_len, (unsigned long)(ISIS_COMMON_HEADER_SIZE+ISIS_IIH_LAN_HEADER_SIZE)));
            return (0);
        }
        ND_TCHECK(*header_iih_lan);
        if (length < ISIS_COMMON_HEADER_SIZE+ISIS_IIH_LAN_HEADER_SIZE)
            goto trunc;
        if (ndo->ndo_vflag == 0) {
            ND_PRINT((ndo, ", src-id %s",
                      isis_print_id(header_iih_lan->source_id, SYSTEM_ID_LEN)));
            ND_PRINT((ndo, ", lan-id %s, prio %u",
                      isis_print_id(header_iih_lan->lan_id,NODE_ID_LEN),
                      header_iih_lan->priority));
            ND_PRINT((ndo, ", length %u", length));
            return (1);
        }
        pdu_len=EXTRACT_16BITS(header_iih_lan->pdu_len);
        if (packet_len>pdu_len) {
           packet_len=pdu_len; /* do TLV decoding as long as it makes sense */
           length=pdu_len;
        }

        ND_PRINT((ndo, "\n\t  source-id: %s,  holding time: %us, Flags: [%s]",
                  isis_print_id(header_iih_lan->source_id,SYSTEM_ID_LEN),
                  EXTRACT_16BITS(header_iih_lan->holding_time),
                  tok2str(isis_iih_circuit_type_values,
                          "unknown circuit type 0x%02x",
                          header_iih_lan->circuit_type)));

        ND_PRINT((ndo, "\n\t  lan-id:    %s, Priority: %u, PDU length: %u",
                  isis_print_id(header_iih_lan->lan_id, NODE_ID_LEN),
                  (header_iih_lan->priority) & ISIS_LAN_PRIORITY_MASK,
                  pdu_len));

        if (ndo->ndo_vflag > 1) {
            if (!print_unknown_data(ndo, pptr, "\n\t  ", ISIS_IIH_LAN_HEADER_SIZE))
                return (0);
        }

        packet_len -= (ISIS_COMMON_HEADER_SIZE+ISIS_IIH_LAN_HEADER_SIZE);
        pptr = p + (ISIS_COMMON_HEADER_SIZE+ISIS_IIH_LAN_HEADER_SIZE);
        break;

    case ISIS_PDU_PTP_IIH:
        if (isis_header->fixed_len != (ISIS_COMMON_HEADER_SIZE+ISIS_IIH_PTP_HEADER_SIZE)) {
            ND_PRINT((ndo, ", bogus fixed header length %u should be %lu",
                      isis_header->fixed_len, (unsigned long)(ISIS_COMMON_HEADER_SIZE+ISIS_IIH_PTP_HEADER_SIZE)));
            return (0);
        }
        ND_TCHECK(*header_iih_ptp);
        if (length < ISIS_COMMON_HEADER_SIZE+ISIS_IIH_PTP_HEADER_SIZE)
            goto trunc;
        if (ndo->ndo_vflag == 0) {
            ND_PRINT((ndo, ", src-id %s", isis_print_id(header_iih_ptp->source_id, SYSTEM_ID_LEN)));
            ND_PRINT((ndo, ", length %u", length));
            return (1);
        }
        pdu_len=EXTRACT_16BITS(header_iih_ptp->pdu_len);
        if (packet_len>pdu_len) {
            packet_len=pdu_len; /* do TLV decoding as long as it makes sense */
            length=pdu_len;
        }

        ND_PRINT((ndo, "\n\t  source-id: %s, holding time: %us, Flags: [%s]",
                  isis_print_id(header_iih_ptp->source_id,SYSTEM_ID_LEN),
                  EXTRACT_16BITS(header_iih_ptp->holding_time),
                  tok2str(isis_iih_circuit_type_values,
                          "unknown circuit type 0x%02x",
                          header_iih_ptp->circuit_type)));

        ND_PRINT((ndo, "\n\t  circuit-id: 0x%02x, PDU length: %u",
                  header_iih_ptp->circuit_id,
                  pdu_len));

        if (ndo->ndo_vflag > 1) {
            if (!print_unknown_data(ndo, pptr, "\n\t  ", ISIS_IIH_PTP_HEADER_SIZE))
                return (0);
        }

        packet_len -= (ISIS_COMMON_HEADER_SIZE+ISIS_IIH_PTP_HEADER_SIZE);
        pptr = p + (ISIS_COMMON_HEADER_SIZE+ISIS_IIH_PTP_HEADER_SIZE);
        break;

    case ISIS_PDU_L1_LSP:
    case ISIS_PDU_L2_LSP:
        if (isis_header->fixed_len != (ISIS_COMMON_HEADER_SIZE+ISIS_LSP_HEADER_SIZE)) {
            ND_PRINT((ndo, ", bogus fixed header length %u should be %lu",
                   isis_header->fixed_len, (unsigned long)ISIS_LSP_HEADER_SIZE));
            return (0);
        }
        ND_TCHECK(*header_lsp);
        if (length < ISIS_COMMON_HEADER_SIZE+ISIS_LSP_HEADER_SIZE)
            goto trunc;
        if (ndo->ndo_vflag == 0) {
            ND_PRINT((ndo, ", lsp-id %s, seq 0x%08x, lifetime %5us",
                      isis_print_id(header_lsp->lsp_id, LSP_ID_LEN),
                      EXTRACT_32BITS(header_lsp->sequence_number),
                      EXTRACT_16BITS(header_lsp->remaining_lifetime)));
            ND_PRINT((ndo, ", length %u", length));
            return (1);
        }
        pdu_len=EXTRACT_16BITS(header_lsp->pdu_len);
        if (packet_len>pdu_len) {
            packet_len=pdu_len; /* do TLV decoding as long as it makes sense */
            length=pdu_len;
        }

        ND_PRINT((ndo, "\n\t  lsp-id: %s, seq: 0x%08x, lifetime: %5us\n\t  chksum: 0x%04x",
               isis_print_id(header_lsp->lsp_id, LSP_ID_LEN),
               EXTRACT_32BITS(header_lsp->sequence_number),
               EXTRACT_16BITS(header_lsp->remaining_lifetime),
               EXTRACT_16BITS(header_lsp->checksum)));

        osi_print_cksum(ndo, (const uint8_t *)header_lsp->lsp_id,
                        EXTRACT_16BITS(header_lsp->checksum),
                        12, length-12);

        ND_PRINT((ndo, ", PDU length: %u, Flags: [ %s",
               pdu_len,
               ISIS_MASK_LSP_OL_BIT(header_lsp->typeblock) ? "Overload bit set, " : ""));

        if (ISIS_MASK_LSP_ATT_BITS(header_lsp->typeblock)) {
            ND_PRINT((ndo, "%s", ISIS_MASK_LSP_ATT_DEFAULT_BIT(header_lsp->typeblock) ? "default " : ""));
            ND_PRINT((ndo, "%s", ISIS_MASK_LSP_ATT_DELAY_BIT(header_lsp->typeblock) ? "delay " : ""));
            ND_PRINT((ndo, "%s", ISIS_MASK_LSP_ATT_EXPENSE_BIT(header_lsp->typeblock) ? "expense " : ""));
            ND_PRINT((ndo, "%s", ISIS_MASK_LSP_ATT_ERROR_BIT(header_lsp->typeblock) ? "error " : ""));
            ND_PRINT((ndo, "ATT bit set, "));
        }
        ND_PRINT((ndo, "%s", ISIS_MASK_LSP_PARTITION_BIT(header_lsp->typeblock) ? "P bit set, " : ""));
        ND_PRINT((ndo, "%s ]", tok2str(isis_lsp_istype_values, "Unknown(0x%x)",
                  ISIS_MASK_LSP_ISTYPE_BITS(header_lsp->typeblock))));

        if (ndo->ndo_vflag > 1) {
            if (!print_unknown_data(ndo, pptr, "\n\t  ", ISIS_LSP_HEADER_SIZE))
                return (0);
        }

        packet_len -= (ISIS_COMMON_HEADER_SIZE+ISIS_LSP_HEADER_SIZE);
        pptr = p + (ISIS_COMMON_HEADER_SIZE+ISIS_LSP_HEADER_SIZE);
        break;

    case ISIS_PDU_L1_CSNP:
    case ISIS_PDU_L2_CSNP:
        if (isis_header->fixed_len != (ISIS_COMMON_HEADER_SIZE+ISIS_CSNP_HEADER_SIZE)) {
            ND_PRINT((ndo, ", bogus fixed header length %u should be %lu",
                      isis_header->fixed_len, (unsigned long)(ISIS_COMMON_HEADER_SIZE+ISIS_CSNP_HEADER_SIZE)));
            return (0);
        }
        ND_TCHECK(*header_csnp);
        if (length < ISIS_COMMON_HEADER_SIZE+ISIS_CSNP_HEADER_SIZE)
            goto trunc;
        if (ndo->ndo_vflag == 0) {
            ND_PRINT((ndo, ", src-id %s", isis_print_id(header_csnp->source_id, NODE_ID_LEN)));
            ND_PRINT((ndo, ", length %u", length));
            return (1);
        }
        pdu_len=EXTRACT_16BITS(header_csnp->pdu_len);
        if (packet_len>pdu_len) {
            packet_len=pdu_len; /* do TLV decoding as long as it makes sense */
            length=pdu_len;
        }

        ND_PRINT((ndo, "\n\t  source-id:    %s, PDU length: %u",
               isis_print_id(header_csnp->source_id, NODE_ID_LEN),
               pdu_len));
        ND_PRINT((ndo, "\n\t  start lsp-id: %s",
               isis_print_id(header_csnp->start_lsp_id, LSP_ID_LEN)));
        ND_PRINT((ndo, "\n\t  end lsp-id:   %s",
               isis_print_id(header_csnp->end_lsp_id, LSP_ID_LEN)));

        if (ndo->ndo_vflag > 1) {
            if (!print_unknown_data(ndo, pptr, "\n\t  ", ISIS_CSNP_HEADER_SIZE))
                return (0);
        }

        packet_len -= (ISIS_COMMON_HEADER_SIZE+ISIS_CSNP_HEADER_SIZE);
        pptr = p + (ISIS_COMMON_HEADER_SIZE+ISIS_CSNP_HEADER_SIZE);
        break;

    case ISIS_PDU_L1_PSNP:
    case ISIS_PDU_L2_PSNP:
        if (isis_header->fixed_len != (ISIS_COMMON_HEADER_SIZE+ISIS_PSNP_HEADER_SIZE)) {
            ND_PRINT((ndo, "- bogus fixed header length %u should be %lu",
                   isis_header->fixed_len, (unsigned long)(ISIS_COMMON_HEADER_SIZE+ISIS_PSNP_HEADER_SIZE)));
            return (0);
        }
        ND_TCHECK(*header_psnp);
        if (length < ISIS_COMMON_HEADER_SIZE+ISIS_PSNP_HEADER_SIZE)
            goto trunc;
        if (ndo->ndo_vflag == 0) {
            ND_PRINT((ndo, ", src-id %s", isis_print_id(header_psnp->source_id, NODE_ID_LEN)));
            ND_PRINT((ndo, ", length %u", length));
            return (1);
        }
        pdu_len=EXTRACT_16BITS(header_psnp->pdu_len);
        if (packet_len>pdu_len) {
            packet_len=pdu_len; /* do TLV decoding as long as it makes sense */
            length=pdu_len;
        }

        ND_PRINT((ndo, "\n\t  source-id:    %s, PDU length: %u",
               isis_print_id(header_psnp->source_id, NODE_ID_LEN),
               pdu_len));

        if (ndo->ndo_vflag > 1) {
            if (!print_unknown_data(ndo, pptr, "\n\t  ", ISIS_PSNP_HEADER_SIZE))
                return (0);
        }

        packet_len -= (ISIS_COMMON_HEADER_SIZE+ISIS_PSNP_HEADER_SIZE);
        pptr = p + (ISIS_COMMON_HEADER_SIZE+ISIS_PSNP_HEADER_SIZE);
        break;

    default:
        if (ndo->ndo_vflag == 0) {
            ND_PRINT((ndo, ", length %u", length));
            return (1);
        }
	(void)print_unknown_data(ndo, pptr, "\n\t  ", length);
	return (0);
    }

    /*
     * Now print the TLV's.
     */

    while (packet_len > 0) {
	ND_TCHECK2(*pptr, 2);
	if (packet_len < 2)
	    goto trunc;
	tlv_type = *pptr++;
	tlv_len = *pptr++;
        tmp =tlv_len; /* copy temporary len & pointer to packet data */
        tptr = pptr;
	packet_len -= 2;

        /* first lets see if we know the TLVs name*/
	ND_PRINT((ndo, "\n\t    %s TLV #%u, length: %u",
               tok2str(isis_tlv_values,
                       "unknown",
                       tlv_type),
               tlv_type,
               tlv_len));

        if (tlv_len == 0) /* something is invalid */
	    continue;

	if (packet_len < tlv_len)
	    goto trunc;

        /* now check if we have a decoder otherwise do a hexdump at the end*/
	switch (tlv_type) {
	case ISIS_TLV_AREA_ADDR:
 	    ND_TCHECK2(*tptr, 1);
 	    alen = *tptr++;
 	    while (tmp && alen < tmp) {
	        ND_TCHECK2(*tptr, alen);
 		ND_PRINT((ndo, "\n\t      Area address (length: %u): %s",
                        alen,
                        isonsap_string(ndo, tptr, alen)));
		tptr += alen;
		tmp -= alen + 1;
		if (tmp==0) /* if this is the last area address do not attemt a boundary check */
                    break;
		ND_TCHECK2(*tptr, 1);
		alen = *tptr++;
	    }
	    break;
	case ISIS_TLV_ISNEIGH:
	    while (tmp >= ETHER_ADDR_LEN) {
                ND_TCHECK2(*tptr, ETHER_ADDR_LEN);
                ND_PRINT((ndo, "\n\t      SNPA: %s", isis_print_id(tptr, ETHER_ADDR_LEN)));
                tmp -= ETHER_ADDR_LEN;
                tptr += ETHER_ADDR_LEN;
	    }
	    break;

        case ISIS_TLV_ISNEIGH_VARLEN:
            if (!ND_TTEST2(*tptr, 1) || tmp < 3) /* min. TLV length */
		goto trunctlv;
	    lan_alen = *tptr++; /* LAN address length */
	    if (lan_alen == 0) {
                ND_PRINT((ndo, "\n\t      LAN address length 0 bytes (invalid)"));
                break;
            }
            tmp --;
            ND_PRINT((ndo, "\n\t      LAN address length %u bytes ", lan_alen));
	    while (tmp >= lan_alen) {
                ND_TCHECK2(*tptr, lan_alen);
                ND_PRINT((ndo, "\n\t\tIS Neighbor: %s", isis_print_id(tptr, lan_alen)));
                tmp -= lan_alen;
                tptr +=lan_alen;
            }
            break;

	case ISIS_TLV_PADDING:
	    break;

        case ISIS_TLV_MT_IS_REACH:
            mt_len = isis_print_mtid(ndo, tptr, "\n\t      ");
            if (mt_len == 0) /* did something go wrong ? */
                goto trunctlv;
            tptr+=mt_len;
            tmp-=mt_len;
            while (tmp >= 2+NODE_ID_LEN+3+1) {
                ext_is_len = isis_print_ext_is_reach(ndo, tptr, "\n\t      ", tlv_type);
                if (ext_is_len == 0) /* did something go wrong ? */
                    goto trunctlv;

                tmp-=ext_is_len;
                tptr+=ext_is_len;
            }
            break;

        case ISIS_TLV_IS_ALIAS_ID:
	    while (tmp >= NODE_ID_LEN+1) { /* is it worth attempting a decode ? */
	        ext_is_len = isis_print_ext_is_reach(ndo, tptr, "\n\t      ", tlv_type);
		if (ext_is_len == 0) /* did something go wrong ? */
	            goto trunctlv;
		tmp-=ext_is_len;
		tptr+=ext_is_len;
	    }
	    break;

        case ISIS_TLV_EXT_IS_REACH:
            while (tmp >= NODE_ID_LEN+3+1) { /* is it worth attempting a decode ? */
                ext_is_len = isis_print_ext_is_reach(ndo, tptr, "\n\t      ", tlv_type);
                if (ext_is_len == 0) /* did something go wrong ? */
                    goto trunctlv;
                tmp-=ext_is_len;
                tptr+=ext_is_len;
            }
            break;
        case ISIS_TLV_IS_REACH:
	    ND_TCHECK2(*tptr,1);  /* check if there is one byte left to read out the virtual flag */
            ND_PRINT((ndo, "\n\t      %s",
                   tok2str(isis_is_reach_virtual_values,
                           "bogus virtual flag 0x%02x",
                           *tptr++)));
	    tlv_is_reach = (const struct isis_tlv_is_reach *)tptr;
            while (tmp >= sizeof(struct isis_tlv_is_reach)) {
		ND_TCHECK(*tlv_is_reach);
		ND_PRINT((ndo, "\n\t      IS Neighbor: %s",
		       isis_print_id(tlv_is_reach->neighbor_nodeid, NODE_ID_LEN)));
		isis_print_metric_block(ndo, &tlv_is_reach->isis_metric_block);
		tmp -= sizeof(struct isis_tlv_is_reach);
		tlv_is_reach++;
	    }
            break;

        case ISIS_TLV_ESNEIGH:
	    tlv_es_reach = (const struct isis_tlv_es_reach *)tptr;
            while (tmp >= sizeof(struct isis_tlv_es_reach)) {
		ND_TCHECK(*tlv_es_reach);
		ND_PRINT((ndo, "\n\t      ES Neighbor: %s",
                       isis_print_id(tlv_es_reach->neighbor_sysid, SYSTEM_ID_LEN)));
		isis_print_metric_block(ndo, &tlv_es_reach->isis_metric_block);
		tmp -= sizeof(struct isis_tlv_es_reach);
		tlv_es_reach++;
	    }
            break;

            /* those two TLVs share the same format */
	case ISIS_TLV_INT_IP_REACH:
	case ISIS_TLV_EXT_IP_REACH:
		if (!isis_print_tlv_ip_reach(ndo, pptr, "\n\t      ", tlv_len))
			return (1);
		break;

	case ISIS_TLV_EXTD_IP_REACH:
	    while (tmp>0) {
                ext_ip_len = isis_print_extd_ip_reach(ndo, tptr, "\n\t      ", AF_INET);
                if (ext_ip_len == 0) /* did something go wrong ? */
                    goto trunctlv;
                tptr+=ext_ip_len;
		tmp-=ext_ip_len;
	    }
	    break;

        case ISIS_TLV_MT_IP_REACH:
            mt_len = isis_print_mtid(ndo, tptr, "\n\t      ");
            if (mt_len == 0) { /* did something go wrong ? */
                goto trunctlv;
            }
            tptr+=mt_len;
            tmp-=mt_len;

            while (tmp>0) {
                ext_ip_len = isis_print_extd_ip_reach(ndo, tptr, "\n\t      ", AF_INET);
                if (ext_ip_len == 0) /* did something go wrong ? */
                    goto trunctlv;
                tptr+=ext_ip_len;
		tmp-=ext_ip_len;
	    }
	    break;

	case ISIS_TLV_IP6_REACH:
	    while (tmp>0) {
                ext_ip_len = isis_print_extd_ip_reach(ndo, tptr, "\n\t      ", AF_INET6);
                if (ext_ip_len == 0) /* did something go wrong ? */
                    goto trunctlv;
                tptr+=ext_ip_len;
		tmp-=ext_ip_len;
	    }
	    break;

	case ISIS_TLV_MT_IP6_REACH:
            mt_len = isis_print_mtid(ndo, tptr, "\n\t      ");
            if (mt_len == 0) { /* did something go wrong ? */
                goto trunctlv;
            }
            tptr+=mt_len;
            tmp-=mt_len;

	    while (tmp>0) {
                ext_ip_len = isis_print_extd_ip_reach(ndo, tptr, "\n\t      ", AF_INET6);
                if (ext_ip_len == 0) /* did something go wrong ? */
                    goto trunctlv;
                tptr+=ext_ip_len;
		tmp-=ext_ip_len;
	    }
	    break;

	case ISIS_TLV_IP6ADDR:
	    while (tmp>=sizeof(struct in6_addr)) {
		ND_TCHECK2(*tptr, sizeof(struct in6_addr));

                ND_PRINT((ndo, "\n\t      IPv6 interface address: %s",
		       ip6addr_string(ndo, tptr)));

		tptr += sizeof(struct in6_addr);
		tmp -= sizeof(struct in6_addr);
	    }
	    break;
	case ISIS_TLV_AUTH:
	    ND_TCHECK2(*tptr, 1);

            ND_PRINT((ndo, "\n\t      %s: ",
                   tok2str(isis_subtlv_auth_values,
                           "unknown Authentication type 0x%02x",
                           *tptr)));

	    switch (*tptr) {
	    case ISIS_SUBTLV_AUTH_SIMPLE:
		if (fn_printzp(ndo, tptr + 1, tlv_len - 1, ndo->ndo_snapend))
		    goto trunctlv;
		break;
	    case ISIS_SUBTLV_AUTH_MD5:
		for(i=1;i<tlv_len;i++) {
		    ND_TCHECK2(*(tptr + i), 1);
		    ND_PRINT((ndo, "%02x", *(tptr + i)));
		}
		if (tlv_len != ISIS_SUBTLV_AUTH_MD5_LEN+1)
                    ND_PRINT((ndo, ", (invalid subTLV) "));

                sigcheck = signature_verify(ndo, optr, length, tptr + 1,
                                            isis_clear_checksum_lifetime,
                                            header_lsp);
                ND_PRINT((ndo, " (%s)", tok2str(signature_check_values, "Unknown", sigcheck)));

		break;
            case ISIS_SUBTLV_AUTH_GENERIC:
		ND_TCHECK2(*(tptr + 1), 2);
                key_id = EXTRACT_16BITS((tptr+1));
                ND_PRINT((ndo, "%u, password: ", key_id));
                for(i=1 + sizeof(uint16_t);i<tlv_len;i++) {
                    ND_TCHECK2(*(tptr + i), 1);
                    ND_PRINT((ndo, "%02x", *(tptr + i)));
                }
                break;
	    case ISIS_SUBTLV_AUTH_PRIVATE:
	    default:
		if (!print_unknown_data(ndo, tptr + 1, "\n\t\t  ", tlv_len - 1))
		    return(0);
		break;
	    }
	    break;

	case ISIS_TLV_PTP_ADJ:
	    tlv_ptp_adj = (const struct isis_tlv_ptp_adj *)tptr;
	    if(tmp>=1) {
		ND_TCHECK2(*tptr, 1);
		ND_PRINT((ndo, "\n\t      Adjacency State: %s (%u)",
		       tok2str(isis_ptp_adjancey_values, "unknown", *tptr),
                        *tptr));
		tmp--;
	    }
	    if(tmp>sizeof(tlv_ptp_adj->extd_local_circuit_id)) {
		ND_TCHECK(tlv_ptp_adj->extd_local_circuit_id);
		ND_PRINT((ndo, "\n\t      Extended Local circuit-ID: 0x%08x",
		       EXTRACT_32BITS(tlv_ptp_adj->extd_local_circuit_id)));
		tmp-=sizeof(tlv_ptp_adj->extd_local_circuit_id);
	    }
	    if(tmp>=SYSTEM_ID_LEN) {
		ND_TCHECK2(tlv_ptp_adj->neighbor_sysid, SYSTEM_ID_LEN);
		ND_PRINT((ndo, "\n\t      Neighbor System-ID: %s",
		       isis_print_id(tlv_ptp_adj->neighbor_sysid, SYSTEM_ID_LEN)));
		tmp-=SYSTEM_ID_LEN;
	    }
	    if(tmp>=sizeof(tlv_ptp_adj->neighbor_extd_local_circuit_id)) {
		ND_TCHECK(tlv_ptp_adj->neighbor_extd_local_circuit_id);
		ND_PRINT((ndo, "\n\t      Neighbor Extended Local circuit-ID: 0x%08x",
		       EXTRACT_32BITS(tlv_ptp_adj->neighbor_extd_local_circuit_id)));
	    }
	    break;

	case ISIS_TLV_PROTOCOLS:
	    ND_PRINT((ndo, "\n\t      NLPID(s): "));
	    while (tmp>0) {
		ND_TCHECK2(*(tptr), 1);
		ND_PRINT((ndo, "%s (0x%02x)",
                       tok2str(nlpid_values,
                               "unknown",
                               *tptr),
                       *tptr));
		if (tmp>1) /* further NPLIDs ? - put comma */
		    ND_PRINT((ndo, ", "));
                tptr++;
                tmp--;
	    }
	    break;

    case ISIS_TLV_MT_PORT_CAP:
    {
      ND_TCHECK2(*(tptr), 2);

      ND_PRINT((ndo, "\n\t       RES: %d, MTID(s): %d",
              (EXTRACT_16BITS (tptr) >> 12),
              (EXTRACT_16BITS (tptr) & 0x0fff)));

      tmp = tmp-2;
      tptr = tptr+2;

      if (tmp)
        isis_print_mt_port_cap_subtlv(ndo, tptr, tmp);

      break;
    }

    case ISIS_TLV_MT_CAPABILITY:

      ND_TCHECK2(*(tptr), 2);

      ND_PRINT((ndo, "\n\t      O: %d, RES: %d, MTID(s): %d",
                (EXTRACT_16BITS(tptr) >> 15) & 0x01,
                (EXTRACT_16BITS(tptr) >> 12) & 0x07,
                EXTRACT_16BITS(tptr) & 0x0fff));

      tmp = tmp-2;
      tptr = tptr+2;

      if (tmp)
        isis_print_mt_capability_subtlv(ndo, tptr, tmp);

      break;

	case ISIS_TLV_TE_ROUTER_ID:
	    ND_TCHECK2(*pptr, sizeof(struct in_addr));
	    ND_PRINT((ndo, "\n\t      Traffic Engineering Router ID: %s", ipaddr_string(ndo, pptr)));
	    break;

	case ISIS_TLV_IPADDR:
	    while (tmp>=sizeof(struct in_addr)) {
		ND_TCHECK2(*tptr, sizeof(struct in_addr));
		ND_PRINT((ndo, "\n\t      IPv4 interface address: %s", ipaddr_string(ndo, tptr)));
		tptr += sizeof(struct in_addr);
		tmp -= sizeof(struct in_addr);
	    }
	    break;

	case ISIS_TLV_HOSTNAME:
	    ND_PRINT((ndo, "\n\t      Hostname: "));
	    if (fn_printzp(ndo, tptr, tmp, ndo->ndo_snapend))
		goto trunctlv;
	    break;

	case ISIS_TLV_SHARED_RISK_GROUP:
	    if (tmp < NODE_ID_LEN)
	        break;
	    ND_TCHECK2(*tptr, NODE_ID_LEN);
	    ND_PRINT((ndo, "\n\t      IS Neighbor: %s", isis_print_id(tptr, NODE_ID_LEN)));
	    tptr+=(NODE_ID_LEN);
	    tmp-=(NODE_ID_LEN);

	    if (tmp < 1)
	        break;
	    ND_TCHECK2(*tptr, 1);
	    ND_PRINT((ndo, ", Flags: [%s]", ISIS_MASK_TLV_SHARED_RISK_GROUP(*tptr++) ? "numbered" : "unnumbered"));
	    tmp--;

	    if (tmp < sizeof(struct in_addr))
	        break;
	    ND_TCHECK2(*tptr, sizeof(struct in_addr));
	    ND_PRINT((ndo, "\n\t      IPv4 interface address: %s", ipaddr_string(ndo, tptr)));
	    tptr+=sizeof(struct in_addr);
	    tmp-=sizeof(struct in_addr);

	    if (tmp < sizeof(struct in_addr))
	        break;
	    ND_TCHECK2(*tptr, sizeof(struct in_addr));
	    ND_PRINT((ndo, "\n\t      IPv4 neighbor address: %s", ipaddr_string(ndo, tptr)));
	    tptr+=sizeof(struct in_addr);
	    tmp-=sizeof(struct in_addr);

	    while (tmp>=4) {
                ND_TCHECK2(*tptr, 4);
                ND_PRINT((ndo, "\n\t      Link-ID: 0x%08x", EXTRACT_32BITS(tptr)));
                tptr+=4;
                tmp-=4;
	    }
	    break;

	case ISIS_TLV_LSP:
	    tlv_lsp = (const struct isis_tlv_lsp *)tptr;
	    while(tmp>=sizeof(struct isis_tlv_lsp)) {
		ND_TCHECK((tlv_lsp->lsp_id)[LSP_ID_LEN-1]);
		ND_PRINT((ndo, "\n\t      lsp-id: %s",
                       isis_print_id(tlv_lsp->lsp_id, LSP_ID_LEN)));
		ND_TCHECK2(tlv_lsp->sequence_number, 4);
		ND_PRINT((ndo, ", seq: 0x%08x", EXTRACT_32BITS(tlv_lsp->sequence_number)));
		ND_TCHECK2(tlv_lsp->remaining_lifetime, 2);
		ND_PRINT((ndo, ", lifetime: %5ds", EXTRACT_16BITS(tlv_lsp->remaining_lifetime)));
		ND_TCHECK2(tlv_lsp->checksum, 2);
		ND_PRINT((ndo, ", chksum: 0x%04x", EXTRACT_16BITS(tlv_lsp->checksum)));
		tmp-=sizeof(struct isis_tlv_lsp);
		tlv_lsp++;
	    }
	    break;

	case ISIS_TLV_CHECKSUM:
	    if (tmp < ISIS_TLV_CHECKSUM_MINLEN)
	        break;
	    ND_TCHECK2(*tptr, ISIS_TLV_CHECKSUM_MINLEN);
	    ND_PRINT((ndo, "\n\t      checksum: 0x%04x ", EXTRACT_16BITS(tptr)));
            /* do not attempt to verify the checksum if it is zero
             * most likely a HMAC-MD5 TLV is also present and
             * to avoid conflicts the checksum TLV is zeroed.
             * see rfc3358 for details
             */
            osi_print_cksum(ndo, optr, EXTRACT_16BITS(tptr), tptr-optr,
                length);
	    break;

	case ISIS_TLV_POI:
	    if (tlv_len >= SYSTEM_ID_LEN + 1) {
		ND_TCHECK2(*tptr, SYSTEM_ID_LEN + 1);
		ND_PRINT((ndo, "\n\t      Purge Originator System-ID: %s",
		       isis_print_id(tptr + 1, SYSTEM_ID_LEN)));
	    }

	    if (tlv_len == 2 * SYSTEM_ID_LEN + 1) {
		ND_TCHECK2(*tptr, 2 * SYSTEM_ID_LEN + 1);
		ND_PRINT((ndo, "\n\t      Received from System-ID: %s",
		       isis_print_id(tptr + SYSTEM_ID_LEN + 1, SYSTEM_ID_LEN)));
	    }
	    break;

	case ISIS_TLV_MT_SUPPORTED:
            if (tmp < ISIS_TLV_MT_SUPPORTED_MINLEN)
                break;
	    while (tmp>1) {
		/* length can only be a multiple of 2, otherwise there is
		   something broken -> so decode down until length is 1 */
		if (tmp!=1) {
                    mt_len = isis_print_mtid(ndo, tptr, "\n\t      ");
                    if (mt_len == 0) /* did something go wrong ? */
                        goto trunctlv;
                    tptr+=mt_len;
                    tmp-=mt_len;
		} else {
		    ND_PRINT((ndo, "\n\t      invalid MT-ID"));
		    break;
		}
	    }
	    break;

	case ISIS_TLV_RESTART_SIGNALING:
            /* first attempt to decode the flags */
            if (tmp < ISIS_TLV_RESTART_SIGNALING_FLAGLEN)
                break;
            ND_TCHECK2(*tptr, ISIS_TLV_RESTART_SIGNALING_FLAGLEN);
            ND_PRINT((ndo, "\n\t      Flags [%s]",
                   bittok2str(isis_restart_flag_values, "none", *tptr)));
            tptr+=ISIS_TLV_RESTART_SIGNALING_FLAGLEN;
            tmp-=ISIS_TLV_RESTART_SIGNALING_FLAGLEN;

            /* is there anything other than the flags field? */
            if (tmp == 0)
                break;

            if (tmp < ISIS_TLV_RESTART_SIGNALING_HOLDTIMELEN)
                break;
            ND_TCHECK2(*tptr, ISIS_TLV_RESTART_SIGNALING_HOLDTIMELEN);

            ND_PRINT((ndo, ", Remaining holding time %us", EXTRACT_16BITS(tptr)));
            tptr+=ISIS_TLV_RESTART_SIGNALING_HOLDTIMELEN;
            tmp-=ISIS_TLV_RESTART_SIGNALING_HOLDTIMELEN;

            /* is there an additional sysid field present ?*/
            if (tmp == SYSTEM_ID_LEN) {
                    ND_TCHECK2(*tptr, SYSTEM_ID_LEN);
                    ND_PRINT((ndo, ", for %s", isis_print_id(tptr,SYSTEM_ID_LEN)));
            }
	    break;

        case ISIS_TLV_IDRP_INFO:
	    if (tmp < ISIS_TLV_IDRP_INFO_MINLEN)
	        break;
            ND_TCHECK2(*tptr, ISIS_TLV_IDRP_INFO_MINLEN);
            ND_PRINT((ndo, "\n\t      Inter-Domain Information Type: %s",
                   tok2str(isis_subtlv_idrp_values,
                           "Unknown (0x%02x)",
                           *tptr)));
            switch (*tptr++) {
            case ISIS_SUBTLV_IDRP_ASN:
                ND_TCHECK2(*tptr, 2); /* fetch AS number */
                ND_PRINT((ndo, "AS Number: %u", EXTRACT_16BITS(tptr)));
                break;
            case ISIS_SUBTLV_IDRP_LOCAL:
            case ISIS_SUBTLV_IDRP_RES:
            default:
                if (!print_unknown_data(ndo, tptr, "\n\t      ", tlv_len - 1))
                    return(0);
                break;
            }
            break;

        case ISIS_TLV_LSP_BUFFERSIZE:
	    if (tmp < ISIS_TLV_LSP_BUFFERSIZE_MINLEN)
	        break;
            ND_TCHECK2(*tptr, ISIS_TLV_LSP_BUFFERSIZE_MINLEN);
            ND_PRINT((ndo, "\n\t      LSP Buffersize: %u", EXTRACT_16BITS(tptr)));
            break;

        case ISIS_TLV_PART_DIS:
            while (tmp >= SYSTEM_ID_LEN) {
                ND_TCHECK2(*tptr, SYSTEM_ID_LEN);
                ND_PRINT((ndo, "\n\t      %s", isis_print_id(tptr, SYSTEM_ID_LEN)));
                tptr+=SYSTEM_ID_LEN;
                tmp-=SYSTEM_ID_LEN;
            }
            break;

        case ISIS_TLV_PREFIX_NEIGH:
	    if (tmp < sizeof(struct isis_metric_block))
	        break;
            ND_TCHECK2(*tptr, sizeof(struct isis_metric_block));
            ND_PRINT((ndo, "\n\t      Metric Block"));
            isis_print_metric_block(ndo, (const struct isis_metric_block *)tptr);
            tptr+=sizeof(struct isis_metric_block);
            tmp-=sizeof(struct isis_metric_block);

            while(tmp>0) {
                ND_TCHECK2(*tptr, 1);
                prefix_len=*tptr++; /* read out prefix length in semioctets*/
                if (prefix_len < 2) {
                    ND_PRINT((ndo, "\n\t\tAddress: prefix length %u < 2", prefix_len));
                    break;
                }
                tmp--;
                if (tmp < prefix_len/2)
                    break;
                ND_TCHECK2(*tptr, prefix_len / 2);
                ND_PRINT((ndo, "\n\t\tAddress: %s/%u",
                       isonsap_string(ndo, tptr, prefix_len / 2), prefix_len * 4));
                tptr+=prefix_len/2;
                tmp-=prefix_len/2;
            }
            break;

        case ISIS_TLV_IIH_SEQNR:
	    if (tmp < ISIS_TLV_IIH_SEQNR_MINLEN)
	        break;
            ND_TCHECK2(*tptr, ISIS_TLV_IIH_SEQNR_MINLEN); /* check if four bytes are on the wire */
            ND_PRINT((ndo, "\n\t      Sequence number: %u", EXTRACT_32BITS(tptr)));
            break;

        case ISIS_TLV_VENDOR_PRIVATE:
	    if (tmp < ISIS_TLV_VENDOR_PRIVATE_MINLEN)
	        break;
            ND_TCHECK2(*tptr, ISIS_TLV_VENDOR_PRIVATE_MINLEN); /* check if enough byte for a full oui */
            vendor_id = EXTRACT_24BITS(tptr);
            ND_PRINT((ndo, "\n\t      Vendor: %s (%u)",
                   tok2str(oui_values, "Unknown", vendor_id),
                   vendor_id));
            tptr+=3;
            tmp-=3;
            if (tmp > 0) /* hexdump the rest */
                if (!print_unknown_data(ndo, tptr, "\n\t\t", tmp))
                    return(0);
            break;
            /*
             * FIXME those are the defined TLVs that lack a decoder
             * you are welcome to contribute code ;-)
             */

        case ISIS_TLV_DECNET_PHASE4:
        case ISIS_TLV_LUCENT_PRIVATE:
        case ISIS_TLV_IPAUTH:
        case ISIS_TLV_NORTEL_PRIVATE1:
        case ISIS_TLV_NORTEL_PRIVATE2:

	default:
		if (ndo->ndo_vflag <= 1) {
			if (!print_unknown_data(ndo, pptr, "\n\t\t", tlv_len))
				return(0);
		}
		break;
	}
        /* do we want to see an additionally hexdump ? */
	if (ndo->ndo_vflag> 1) {
		if (!print_unknown_data(ndo, pptr, "\n\t      ", tlv_len))
			return(0);
	}

	pptr += tlv_len;
	packet_len -= tlv_len;
    }

    if (packet_len != 0) {
	ND_PRINT((ndo, "\n\t      %u straggler bytes", packet_len));
    }
    return (1);

 trunc:
    ND_PRINT((ndo, "%s", tstr));
    return (1);

 trunctlv:
    ND_PRINT((ndo, "\n\t\t"));
    ND_PRINT((ndo, "%s", tstr));
    return(1);
}
