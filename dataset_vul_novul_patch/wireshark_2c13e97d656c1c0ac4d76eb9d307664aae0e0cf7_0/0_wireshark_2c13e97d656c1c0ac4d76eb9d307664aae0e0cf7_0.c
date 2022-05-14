dissect_ppi(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    proto_tree    *ppi_tree    = NULL, *ppi_flags_tree = NULL, *seg_tree = NULL, *ampdu_tree = NULL;
    proto_tree    *agg_tree    = NULL;
    proto_item    *ti          = NULL;
    tvbuff_t      *next_tvb;
    int            offset      = 0;
    guint          version, flags;
    gint           tot_len, data_len;
    guint          data_type;
    guint32        dlt;
    guint32        n_ext_flags = 0;
    guint32        ampdu_id    = 0;
    fragment_head *fd_head     = NULL;
    fragment_item *ft_fdh      = NULL;
    gint           mpdu_count  = 0;
    gchar         *mpdu_str;
    gboolean       first_mpdu  = TRUE;
     guint          last_frame  = 0;
     gint len_remain, /*pad_len = 0,*/ ampdu_len = 0;
     struct ieee_802_11_phdr phdr;
    int            wtap_encap;
    struct eth_phdr eth;
    void          *phdrp;
 
     col_set_str(pinfo->cinfo, COL_PROTOCOL, "PPI");
     col_clear(pinfo->cinfo, COL_INFO);

    version = tvb_get_guint8(tvb, offset);
    flags = tvb_get_guint8(tvb, offset + 1);

    tot_len = tvb_get_letohs(tvb, offset+2);
    dlt = tvb_get_letohl(tvb, offset+4);

    col_add_fstr(pinfo->cinfo, COL_INFO, "PPI version %u, %u bytes",
                 version, tot_len);

    /* Dissect the packet */
    if (tree) {
        ti = proto_tree_add_protocol_format(tree, proto_ppi,
                                            tvb, 0, tot_len, "PPI version %u, %u bytes", version, tot_len);
        ppi_tree = proto_item_add_subtree(ti, ett_ppi_pph);
        proto_tree_add_item(ppi_tree, hf_ppi_head_version,
                            tvb, offset, 1, ENC_LITTLE_ENDIAN);

        ti = proto_tree_add_item(ppi_tree, hf_ppi_head_flags,
                                 tvb, offset + 1, 1, ENC_LITTLE_ENDIAN);
        ppi_flags_tree = proto_item_add_subtree(ti, ett_ppi_flags);
        proto_tree_add_item(ppi_flags_tree, hf_ppi_head_flag_alignment,
                            tvb, offset + 1, 1, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(ppi_flags_tree, hf_ppi_head_flag_reserved,
                            tvb, offset + 1, 1, ENC_LITTLE_ENDIAN);

        proto_tree_add_item(ppi_tree, hf_ppi_head_len,
                                 tvb, offset + 2, 2, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(ppi_tree, hf_ppi_head_dlt,
                                 tvb, offset + 4, 4, ENC_LITTLE_ENDIAN);
    }

    tot_len -= PPI_V0_HEADER_LEN;
    offset += 8;

    /* We don't have any 802.11 metadata yet. */
    memset(&phdr, 0, sizeof(phdr));
    phdr.fcs_len = -1;
    phdr.decrypted = FALSE;
    phdr.datapad = FALSE;
    phdr.phy = PHDR_802_11_PHY_UNKNOWN;
    phdr.presence_flags = 0;

    while (tot_len > 0) {
        data_type = tvb_get_letohs(tvb, offset);
        data_len = tvb_get_letohs(tvb, offset + 2) + 4;
        tot_len -= data_len;

        switch (data_type) {

        case PPI_80211_COMMON:
            dissect_80211_common(tvb, pinfo, ppi_tree, offset, data_len, &phdr);
            break;

        case PPI_80211N_MAC:
            dissect_80211n_mac(tvb, pinfo, ppi_tree, offset, data_len,
                TRUE, &n_ext_flags, &ampdu_id, &phdr);
            break;

        case PPI_80211N_MAC_PHY:
            dissect_80211n_mac_phy(tvb, pinfo, ppi_tree, offset,
                data_len, &n_ext_flags, &ampdu_id, &phdr);
            break;

        case PPI_SPECTRUM_MAP:
            ADD_BASIC_TAG(hf_spectrum_map);
            break;

        case PPI_PROCESS_INFO:
            ADD_BASIC_TAG(hf_process_info);
            break;

        case PPI_CAPTURE_INFO:
            ADD_BASIC_TAG(hf_capture_info);
            break;

        case PPI_AGGREGATION_EXTENSION:
            dissect_aggregation_extension(tvb, pinfo, ppi_tree, offset, data_len);
            break;

        case PPI_8023_EXTENSION:
            dissect_8023_extension(tvb, pinfo, ppi_tree, offset, data_len);
            break;

        case PPI_GPS_INFO:
            if (ppi_gps_handle == NULL)
            {
                proto_tree_add_item(ppi_tree, hf_ppi_gps, tvb, offset, data_len, ENC_NA);
            }
            else /* we found a suitable dissector */
            {
                /* skip over the ppi_fieldheader, and pass it off to the dedicated GPS dissetor */
                next_tvb = tvb_new_subset(tvb, offset + 4, data_len - 4 , -1);
                call_dissector(ppi_gps_handle, next_tvb, pinfo, ppi_tree);
            }
            break;

        case PPI_VECTOR_INFO:
            if (ppi_vector_handle == NULL)
            {
                proto_tree_add_item(ppi_tree, hf_ppi_vector, tvb, offset, data_len, ENC_NA);
            }
            else /* we found a suitable dissector */
            {
                /* skip over the ppi_fieldheader, and pass it off to the dedicated VECTOR dissetor */
                next_tvb = tvb_new_subset(tvb, offset + 4, data_len - 4 , -1);
                call_dissector(ppi_vector_handle, next_tvb, pinfo, ppi_tree);
            }
            break;

        case PPI_SENSOR_INFO:
            if (ppi_sensor_handle == NULL)
            {
                proto_tree_add_item(ppi_tree, hf_ppi_harris, tvb, offset, data_len, ENC_NA);
            }
            else /* we found a suitable dissector */
            {
                /* skip over the ppi_fieldheader, and pass it off to the dedicated SENSOR dissetor */
                next_tvb = tvb_new_subset(tvb, offset + 4, data_len - 4 , -1);
                call_dissector(ppi_sensor_handle, next_tvb, pinfo, ppi_tree);
            }
            break;

        case PPI_ANTENNA_INFO:
            if (ppi_antenna_handle == NULL)
            {
                proto_tree_add_item(ppi_tree, hf_ppi_antenna, tvb, offset, data_len, ENC_NA);
            }
            else /* we found a suitable dissector */
            {
                /* skip over the ppi_fieldheader, and pass it off to the dedicated ANTENNA dissetor */
                next_tvb = tvb_new_subset(tvb, offset + 4, data_len - 4 , -1);
                call_dissector(ppi_antenna_handle, next_tvb, pinfo, ppi_tree);
            }
            break;

        case FNET_PRIVATE:
            if (ppi_fnet_handle == NULL)
            {
                proto_tree_add_item(ppi_tree, hf_ppi_fnet, tvb, offset, data_len, ENC_NA);
            }
            else /* we found a suitable dissector */
            {
                /* skip over the ppi_fieldheader, and pass it off to the dedicated FNET dissetor */
                next_tvb = tvb_new_subset(tvb, offset + 4, data_len - 4 , -1);
                call_dissector(ppi_fnet_handle, next_tvb, pinfo, ppi_tree);
            }
            break;

        default:
            proto_tree_add_item(ppi_tree, hf_ppi_reserved, tvb, offset, data_len, ENC_NA);
        }

        offset += data_len;
        if (IS_PPI_FLAG_ALIGN(flags)){
            offset += PADDING4(offset);
        }
    }

    if (ppi_ampdu_reassemble && DOT11N_IS_AGGREGATE(n_ext_flags)) {
        len_remain = tvb_captured_length_remaining(tvb, offset);
#if 0 /* XXX: pad_len never actually used ?? */
        if (DOT11N_MORE_AGGREGATES(n_ext_flags)) {
            pad_len = PADDING4(len_remain);
        }
#endif
        pinfo->fragmented = TRUE;

        /* Make sure we aren't going to go past AGGREGATE_MAX
         * and caclulate our full A-MPDU length */
        fd_head = fragment_get(&ampdu_reassembly_table, pinfo, ampdu_id, NULL);
        while (fd_head) {
            ampdu_len += fd_head->len + PADDING4(fd_head->len) + 4;
            fd_head = fd_head->next;
        }
        if (ampdu_len > AGGREGATE_MAX) {
            if (tree) {
                proto_tree_add_expert_format(ppi_tree, pinfo, &ei_ppi_invalid_length, tvb, offset, -1, "Aggregate length greater than maximum (%u)", AGGREGATE_MAX);
                THROW(ReportedBoundsError);
            } else {
                return;
            }
        }

        /*
         * Note that we never actually reassemble our A-MPDUs.  Doing
         * so would require prepending each MPDU with an A-MPDU delimiter
         * and appending it with padding, only to hand it off to some
         * routine which would un-do the work we just did.  We're using
         * the reassembly code to track MPDU sizes and frame numbers.
         */
        /*??fd_head = */fragment_add_seq_next(&ampdu_reassembly_table,
            tvb, offset, pinfo, ampdu_id, NULL, len_remain, TRUE);
        pinfo->fragmented = TRUE;

        /* Do reassembly? */
        fd_head = fragment_get(&ampdu_reassembly_table, pinfo, ampdu_id, NULL);

        /* Show our fragments */
        if (fd_head && tree) {
            ft_fdh = fd_head;
            /* List our fragments */
            seg_tree = proto_tree_add_subtree_format(ppi_tree, tvb, offset, -1,
                    ett_ampdu_segments, &ti, "A-MPDU (%u bytes w/hdrs):", ampdu_len);
            PROTO_ITEM_SET_GENERATED(ti);

            while (ft_fdh) {
                if (ft_fdh->tvb_data && ft_fdh->len) {
                    last_frame = ft_fdh->frame;
                    if (!first_mpdu)
                        proto_item_append_text(ti, ",");
                    first_mpdu = FALSE;
                    proto_item_append_text(ti, " #%u(%u)",
                        ft_fdh->frame, ft_fdh->len);
                    proto_tree_add_uint_format(seg_tree, hf_ampdu_segment,
                        tvb, 0, 0, last_frame,
                        "Frame: %u (%u byte%s)",
                        last_frame,
                        ft_fdh->len,
                        plurality(ft_fdh->len, "", "s"));
                }
                ft_fdh = ft_fdh->next;
            }
            if (last_frame && last_frame != pinfo->fd->num)
                proto_tree_add_uint(seg_tree, hf_ampdu_reassembled_in,
                    tvb, 0, 0, last_frame);
        }

        if (fd_head && !DOT11N_MORE_AGGREGATES(n_ext_flags)) {
            if (tree) {
                ti = proto_tree_add_protocol_format(tree,
                    proto_get_id_by_filter_name("wlan_aggregate"),
                    tvb, 0, tot_len, "IEEE 802.11 Aggregate MPDU");
                agg_tree = proto_item_add_subtree(ti, ett_ampdu);
            }

            while (fd_head) {
                if (fd_head->tvb_data && fd_head->len) {
                    mpdu_count++;
                    mpdu_str = wmem_strdup_printf(wmem_packet_scope(), "MPDU #%d", mpdu_count);

                    next_tvb = tvb_new_chain(tvb, fd_head->tvb_data);
                    add_new_data_source(pinfo, next_tvb, mpdu_str);

                    ampdu_tree = proto_tree_add_subtree(agg_tree, next_tvb, 0, -1, ett_ampdu_segment, NULL, mpdu_str);
                    call_dissector_with_data(ieee80211_radio_handle, next_tvb, pinfo, ampdu_tree, &phdr);
                }
                fd_head = fd_head->next;
            }
            proto_tree_add_uint(seg_tree, hf_ampdu_count, tvb, 0, 0, mpdu_count);
            pinfo->fragmented=FALSE;
        } else {
            next_tvb = tvb_new_subset_remaining(tvb, offset);
            col_set_str(pinfo->cinfo, COL_PROTOCOL, "IEEE 802.11n");
            col_set_str(pinfo->cinfo, COL_INFO, "Unreassembled A-MPDU data");
            call_dissector(data_handle, next_tvb, pinfo, tree);
        }
        return;
    }

    next_tvb = tvb_new_subset_remaining(tvb, offset);
    /*
     * You can't just call an arbitrary subdissector based on a
     * LINKTYPE_ value, because they may expect a particular
     * pseudo-header to be passed to them.
     *
     * So we look for LINKTYPE_IEEE802_11, which is 105, and, if
     * that's what the LINKTYPE_ value is, pass it a pointer
     * to a struct ieee_802_11_phdr; otherwise, we pass it
     * a null pointer - if it actually matters, we need to
     * construct the appropriate pseudo-header and pass that.
     */
    if (dlt == 105) {
        /* LINKTYPE_IEEE802_11 */
         call_dissector_with_data(ieee80211_radio_handle, next_tvb, pinfo, tree, &phdr);
     } else {
         /* Everything else.  This will pass a NULL data argument. */
        wtap_encap = wtap_pcap_encap_to_wtap_encap(dlt);
        switch (wtap_encap) {

        case WTAP_ENCAP_ETHERNET:
            eth.fcs_len = -1;    /* Unknown whether we have an FCS */
            phdrp = &eth;
            break;

        default:
            phdrp = NULL;
            break;
        }
        dissector_try_uint_new(wtap_encap_dissector_table,
            wtap_encap, next_tvb, pinfo, tree, TRUE, phdrp);
     }
 }
