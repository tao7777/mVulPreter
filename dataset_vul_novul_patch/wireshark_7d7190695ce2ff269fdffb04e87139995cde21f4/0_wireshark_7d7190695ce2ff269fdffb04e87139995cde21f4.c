fp_set_per_packet_inf_from_conv(umts_fp_conversation_info_t *p_conv_data,
                                tvbuff_t *tvb, packet_info *pinfo,
                                proto_tree *tree _U_)
{
    fp_info  *fpi;
    guint8    tfi, c_t;
    int       offset = 0, i=0, j=0, num_tbs, chan, tb_size, tb_bit_off;
    gboolean  is_control_frame;
    umts_mac_info *macinf;
    rlc_info *rlcinf;
    guint8 fake_lchid=0;
    gint *cur_val=NULL;

    fpi = wmem_new0(wmem_file_scope(), fp_info);
    p_add_proto_data(wmem_file_scope(), pinfo, proto_fp, 0, fpi);

    fpi->iface_type = p_conv_data->iface_type;
    fpi->division = p_conv_data->division;
    fpi->release = 7;               /* Set values greater then the checks performed */
    fpi->release_year = 2006;
    fpi->release_month = 12;
    fpi->channel = p_conv_data->channel;
    fpi->dch_crc_present = p_conv_data->dch_crc_present;
    /*fpi->paging_indications;*/
    fpi->link_type = FP_Link_Ethernet;

#if 0
    /*Only do this the first run, signals that we need to reset the RLC fragtable*/
    if (!pinfo->fd->flags.visited &&  p_conv_data->reset_frag ) {
        fpi->reset_frag = p_conv_data->reset_frag;
        p_conv_data->reset_frag = FALSE;
    }
#endif
    /* remember 'lower' UDP layer port information so we can later
     * differentiate 'lower' UDP layer from 'user data' UDP layer */
    fpi->srcport = pinfo->srcport;
    fpi->destport = pinfo->destport;

    fpi->com_context_id = p_conv_data->com_context_id;

    if (pinfo->link_dir == P2P_DIR_UL) {
        fpi->is_uplink = TRUE;
    } else {
        fpi->is_uplink = FALSE;
    }

    is_control_frame = tvb_get_guint8(tvb, offset) & 0x01;

    switch (fpi->channel) {
        case CHANNEL_HSDSCH: /* HS-DSCH - High Speed Downlink Shared Channel */
            fpi->hsdsch_entity = p_conv_data->hsdsch_entity;
            macinf = wmem_new0(wmem_file_scope(), umts_mac_info);
            fpi->hsdsch_macflowd_id = p_conv_data->hsdsch_macdflow_id;
           macinf->content[0] = hsdsch_macdflow_id_mac_content_map[p_conv_data->hsdsch_macdflow_id]; /*MAC_CONTENT_PS_DTCH;*/
            macinf->lchid[0] = p_conv_data->hsdsch_macdflow_id;
            /*macinf->content[0] = lchId_type_table[p_conv_data->edch_lchId[0]];*/
            p_add_proto_data(wmem_file_scope(), pinfo, proto_umts_mac, 0, macinf);

            rlcinf = wmem_new0(wmem_file_scope(), rlc_info);

            /*Figure out RLC_MODE based on MACd-flow-ID, basically MACd-flow-ID = 0 then it's SRB0 == UM else AM*/
            rlcinf->mode[0] = hsdsch_macdflow_id_rlc_map[p_conv_data->hsdsch_macdflow_id];

            if (fpi->hsdsch_entity == hs /*&& !rlc_is_ciphered(pinfo)*/) {
                for (i=0; i<MAX_NUM_HSDHSCH_MACDFLOW; i++) {
                    /*Figure out if this channel is multiplexed (signaled from RRC)*/
                    if ((cur_val=(gint *)g_tree_lookup(hsdsch_muxed_flows, GINT_TO_POINTER((gint)p_conv_data->hrnti))) != NULL) {
                        j = 1 << i;
                        fpi->hsdhsch_macfdlow_is_mux[i] = j & *cur_val;
                    } else {
                        fpi->hsdhsch_macfdlow_is_mux[i] = FALSE;
                    }

                }
            }
            /* Make configurable ?(available in NBAP?) */
            /* urnti[MAX_RLC_CHANS] */
            /*
            switch (p_conv_data->rlc_mode) {
                case FP_RLC_TM:
                    rlcinf->mode[0] = RLC_TM;
                    break;
                case FP_RLC_UM:
                    rlcinf->mode[0] = RLC_UM;
                    break;
                case FP_RLC_AM:
                    rlcinf->mode[0] = RLC_AM;
                    break;
                case FP_RLC_MODE_UNKNOWN:
                default:
                    rlcinf->mode[0] = RLC_UNKNOWN_MODE;
                    break;
            }*/
            /* rbid[MAX_RLC_CHANS] */
            /* For RLC re-assembly to work we urnti signaled from NBAP */
            rlcinf->urnti[0] = fpi->com_context_id;
            rlcinf->li_size[0] = RLC_LI_7BITS;
            rlcinf->ciphered[0] = FALSE;
            rlcinf->deciphered[0] = FALSE;
            p_add_proto_data(wmem_file_scope(), pinfo, proto_rlc, 0, rlcinf);


            return fpi;

        case CHANNEL_EDCH:
            /*Most configuration is now done in the actual dissecting function*/
            macinf = wmem_new0(wmem_file_scope(), umts_mac_info);
            rlcinf = wmem_new0(wmem_file_scope(), rlc_info);
            fpi->no_ddi_entries = p_conv_data->no_ddi_entries;
            for (i=0; i<fpi->no_ddi_entries; i++) {
                fpi->edch_ddi[i] = p_conv_data->edch_ddi[i];    /*Set the DDI value*/
                fpi->edch_macd_pdu_size[i] = p_conv_data->edch_macd_pdu_size[i];    /*Set the size*/
                fpi->edch_lchId[i] = p_conv_data->edch_lchId[i];    /*Set the channel id for this entry*/
                /*macinf->content[i] = lchId_type_table[p_conv_data->edch_lchId[i]]; */    /*Set the proper Content type for the mac layer.*/
            /*    rlcinf->mode[i] = lchId_rlc_map[p_conv_data->edch_lchId[i]];*/ /* Set RLC mode by lchid to RLC_MODE map in nbap.h */

            }
            fpi->edch_type = p_conv_data->edch_type;

           /* macinf = wmem_new0(wmem_file_scope(), umts_mac_info);
            macinf->content[0] = MAC_CONTENT_PS_DTCH;*/
            p_add_proto_data(wmem_file_scope(), pinfo, proto_umts_mac, 0, macinf);


            /* For RLC re-assembly to work we need a urnti signaled from NBAP */
            rlcinf->urnti[0] = fpi->com_context_id;
           /* rlcinf->mode[0] = RLC_AM;*/
            rlcinf->li_size[0] = RLC_LI_7BITS;
            rlcinf->ciphered[0] = FALSE;
            rlcinf->deciphered[0] = FALSE;

            p_add_proto_data(wmem_file_scope(), pinfo, proto_rlc, 0, rlcinf);

            return fpi;

        case CHANNEL_PCH:
            fpi->paging_indications = p_conv_data->paging_indications;
            fpi->num_chans = p_conv_data->num_dch_in_flow;
            /* Set offset to point to first TFI
             */
            if (is_control_frame) {
                /* control frame, we're done */
                return fpi;
            }
            /* Set offset to TFI */
            offset = 3;
            break;
        case CHANNEL_DCH:
            fpi->num_chans = p_conv_data->num_dch_in_flow;
            if (is_control_frame) {
                /* control frame, we're done */
                return fpi;
            }

            rlcinf = wmem_new0(wmem_file_scope(), rlc_info);
            macinf = wmem_new0(wmem_file_scope(), umts_mac_info);
            offset = 2;    /*To correctly read the tfi*/
            fakes  = 5; /* Reset fake counter. */
            for (chan=0; chan < fpi->num_chans; chan++) {    /*Iterate over the what channels*/
                    /*Iterate over the transport blocks*/
                   /*tfi = tvb_get_guint8(tvb, offset);*/
                   /*TFI is 5 bits according to 3GPP TS 25.321, paragraph 6.2.4.4*/
                    tfi = tvb_get_bits8(tvb, 3+offset*8, 5);

                   /*Figure out the number of tbs and size*/
                   num_tbs = (fpi->is_uplink) ? p_conv_data->fp_dch_channel_info[chan].ul_chan_num_tbs[tfi] : p_conv_data->fp_dch_channel_info[chan].dl_chan_num_tbs[tfi];
                   tb_size=  (fpi->is_uplink) ? p_conv_data->fp_dch_channel_info[i].ul_chan_tf_size[tfi] :    p_conv_data->fp_dch_channel_info[i].dl_chan_tf_size[tfi];

                    /*TODO: This stuff has to be reworked!*/
                    /*Generates a fake logical channel id for non multiplexed channel*/
                    if ( p_conv_data->dchs_in_flow_list[chan] != 31 && (p_conv_data->dchs_in_flow_list[chan] == 24 &&
                     tb_size != 340) ) {
                        fake_lchid = make_fake_lchid(pinfo, p_conv_data->dchs_in_flow_list[chan]);
                    }
                    tb_bit_off = (2+p_conv_data->num_dch_in_flow)*8;    /*Point to the C/T of first TB*/
                    /*Set configuration for individual blocks*/
                    for (j=0; j < num_tbs && j+chan < MAX_MAC_FRAMES; j++) {
                        /*Set transport channel id (useful for debugging)*/
                        macinf->trchid[j+chan] = p_conv_data->dchs_in_flow_list[chan];

                        /*Transport Channel m31 and 24 might be multiplexed!*/
                        if ( p_conv_data->dchs_in_flow_list[chan] == 31 || p_conv_data->dchs_in_flow_list[chan] == 24) {

                            /****** MUST FIGURE OUT IF THIS IS REALLY MULTIPLEXED OR NOT*******/
                            /*If Trchid == 31 and only on TB, we have no multiplexing*/
                            if (0/*p_conv_data->dchs_in_flow_list[chan] == 31 && num_tbs == 1*/) {
                                macinf->ctmux[j+chan] = FALSE;/*Set TRUE if this channel is multiplexed (ie. C/T flag exists)*/

                                macinf->lchid[j+chan] = 1;

                                macinf->content[j+chan] = lchId_type_table[1];    /*Base MAC content on logical channel id (Table is in packet-nbap.h)*/
                                rlcinf->mode[j+chan] = lchId_rlc_map[1];    /*Based RLC mode on logical channel id*/

                            }
                            /*Indicate we don't have multiplexing.*/
                            else if (p_conv_data->dchs_in_flow_list[chan] == 24 && tb_size != 340) {
                                macinf->ctmux[j+chan] = FALSE;/*Set TRUE if this channel is multiplexed (ie. C/T flag exists)*/

                                /*g_warning("settin this for %d", pinfo->num);*/
                                macinf->lchid[j+chan] = fake_lchid;
                                macinf->fake_chid[j+chan] = TRUE;
                                macinf->content[j+chan] = MAC_CONTENT_PS_DTCH; /*lchId_type_table[fake_lchid];*/    /*Base MAC content on logical channel id (Table is in packet-nbap.h)*/
                                rlcinf->mode[j+chan] = RLC_AM;/*lchId_rlc_map[fake_lchid];*/    /*Based RLC mode on logical channel id*/
                            }
                            /*We have multiplexing*/
                            else {
                                macinf->ctmux[j+chan] = TRUE;/*Set TRUE if this channel is multiplexed (ie. C/T flag exists)*/
 
                                 /* Peek at C/T, different RLC params for different logical channels */
                                 /*C/T is 4 bits according to 3GPP TS 25.321, paragraph 9.2.1, from MAC header (not FP)*/
                                c_t = (tvb_get_bits8(tvb, tb_bit_off/*(2+p_conv_data->num_dch_in_flow)*8*/, 4) + 1) % 0xf;    /* c_t = tvb_get_guint8(tvb, offset);*/
                                macinf->lchid[j+chan] = c_t;
 
                                macinf->content[j+chan] = lchId_type_table[c_t];    /*Base MAC content on logical channel id (Table is in packet-nbap.h)*/
                                rlcinf->mode[j+chan] = lchId_rlc_map[c_t];    /*Based RLC mode on logical channel id*/
                             }
                         } else {
                             fake_lchid = make_fake_lchid(pinfo, p_conv_data->dchs_in_flow_list[chan]);
                            macinf->ctmux[j+chan] = FALSE;/*Set TRUE if this channel is multiplexed (ie. C/T flag exists)*/
                            /*macinf->content[j+chan] = MAC_CONTENT_CS_DTCH;*/
                            macinf->content[j+chan] = lchId_type_table[fake_lchid];


                            rlcinf->mode[j+chan] = lchId_rlc_map[fake_lchid];

                            /*Generate virtual logical channel id*/
                            /************************/
                            /*TODO: Once proper lchid is always set, this has to be removed*/
                            macinf->fake_chid[j+chan] = TRUE;
                            macinf->lchid[j+chan] = fake_lchid;  /*make_fake_lchid(pinfo, p_conv_data->dchs_in_flow_list[chan]);*/
                            /************************/
                        }

                        /*** Set rlc info ***/
                        rlcinf->urnti[j+chan] = p_conv_data->com_context_id;
                        rlcinf->li_size[j+chan] = RLC_LI_7BITS;
#if 0
                        /*If this entry exists, SECRUITY_MODE is completed (signled by RRC)*/
                        if ( rrc_ciph_inf && g_tree_lookup(rrc_ciph_inf, GINT_TO_POINTER((gint)p_conv_data->com_context_id)) != NULL ) {
                            rlcinf->ciphered[j+chan] = TRUE;
                        } else {
                            rlcinf->ciphered[j+chan] = FALSE;
                        }
#endif
                        rlcinf->ciphered[j+chan] = FALSE;
                        rlcinf->deciphered[j+chan] = FALSE;
                        rlcinf->rbid[j+chan] = macinf->lchid[j+chan];


                        /*Step over this TB and it's C/T flag.*/
                        tb_bit_off += tb_size+4;
                    }

                    offset++;
            }
            p_add_proto_data(wmem_file_scope(), pinfo, proto_umts_mac, 0, macinf);
            p_add_proto_data(wmem_file_scope(), pinfo, proto_rlc, 0, rlcinf);
            /* Set offset to point to first TFI
             * the Number of TFI's = number of DCH's in the flow
             */
            offset = 2;
            break;
        case CHANNEL_FACH_FDD:
            fpi->num_chans = p_conv_data->num_dch_in_flow;
            if (is_control_frame) {
                /* control frame, we're done */
                return fpi;
            }
            /* Set offset to point to first TFI
             * the Number of TFI's = number of DCH's in the flow
             */
            offset = 2;
            /* Set MAC data */
            macinf = wmem_new0(wmem_file_scope(), umts_mac_info);
            macinf->ctmux[0]   = 1;
            macinf->content[0] = MAC_CONTENT_DCCH;
            p_add_proto_data(wmem_file_scope(), pinfo, proto_umts_mac, 0, macinf);
            /* Set RLC data */
            rlcinf = wmem_new0(wmem_file_scope(), rlc_info);
            /* Make configurable ?(avaliable in NBAP?) */
            /* For RLC re-assembly to work we need to fake urnti */
            rlcinf->urnti[0] = fpi->channel;
            rlcinf->mode[0] = RLC_AM;
            /* rbid[MAX_RLC_CHANS] */
            rlcinf->li_size[0] = RLC_LI_7BITS;
            rlcinf->ciphered[0] = FALSE;
            rlcinf->deciphered[0] = FALSE;
            p_add_proto_data(wmem_file_scope(), pinfo, proto_rlc, 0, rlcinf);
            break;

        case CHANNEL_RACH_FDD:
            fpi->num_chans = p_conv_data->num_dch_in_flow;
            if (is_control_frame) {
                /* control frame, we're done */
                return fpi;
            }
            /* Set offset to point to first TFI
             * the Number of TFI's = number of DCH's in the flow
             */
            offset = 2;
            /* set MAC data */
            macinf = wmem_new0(wmem_file_scope(), umts_mac_info);
            rlcinf = wmem_new0(wmem_file_scope(), rlc_info);
            for ( chan = 0; chan < fpi->num_chans; chan++ ) {
                    macinf->ctmux[chan]   = 1;
                    macinf->content[chan] = MAC_CONTENT_DCCH;
                    rlcinf->urnti[chan] = fpi->com_context_id;    /*Note that MAC probably will change this*/
            }



            p_add_proto_data(wmem_file_scope(), pinfo, proto_umts_mac, 0, macinf);
            p_add_proto_data(wmem_file_scope(), pinfo, proto_rlc, 0, rlcinf);
            break;
        case CHANNEL_HSDSCH_COMMON:
                rlcinf = wmem_new0(wmem_file_scope(), rlc_info);
                macinf = wmem_new0(wmem_file_scope(), umts_mac_info);
                p_add_proto_data(wmem_file_scope(), pinfo, proto_umts_mac, 0, macinf);
                p_add_proto_data(wmem_file_scope(), pinfo, proto_rlc, 0, rlcinf);
            break;
        default:
            expert_add_info(pinfo, NULL, &ei_fp_transport_channel_type_unknown);
            return NULL;
    }

    /* Peek at the packet as the per packet info seems not to take the tfi into account */
    for (i=0; i<fpi->num_chans; i++) {
        tfi = tvb_get_guint8(tvb, offset);

        /*TFI is 5 bits according to 3GPP TS 25.321, paragraph 6.2.4.4*/
        /*tfi = tvb_get_bits8(tvb, offset*8, 5);*/
        if (pinfo->link_dir == P2P_DIR_UL) {
            fpi->chan_tf_size[i] = p_conv_data->fp_dch_channel_info[i].ul_chan_tf_size[tfi];
            fpi->chan_num_tbs[i] = p_conv_data->fp_dch_channel_info[i].ul_chan_num_tbs[tfi];
        } else {
            fpi->chan_tf_size[i] = p_conv_data->fp_dch_channel_info[i].dl_chan_tf_size[tfi];
            fpi->chan_num_tbs[i] = p_conv_data->fp_dch_channel_info[i].dl_chan_num_tbs[tfi];
        }
        offset++;
    }


    return fpi;
}
