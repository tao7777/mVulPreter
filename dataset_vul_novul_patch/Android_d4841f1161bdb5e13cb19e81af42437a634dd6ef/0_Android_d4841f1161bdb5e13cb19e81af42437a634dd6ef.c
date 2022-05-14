WORD32 ih264d_mark_err_slice_skip(dec_struct_t * ps_dec,
                                WORD32 num_mb_skip,
                                UWORD8 u1_is_idr_slice,
                                UWORD16 u2_frame_num,
 pocstruct_t *ps_cur_poc,
                                WORD32 prev_slice_err)
{
    WORD32 i2_cur_mb_addr;
    UWORD32 u1_num_mbs, u1_num_mbsNby2;
    UWORD32 u1_mb_idx = ps_dec->u1_mb_idx;
    UWORD32 i2_mb_skip_run;

    UWORD32 u1_num_mbs_next, u1_end_of_row;
 const UWORD32 i2_pic_wdin_mbs = ps_dec->u2_frm_wd_in_mbs;
    UWORD32 u1_slice_end;
    UWORD32 u1_tfr_n_mb;
    UWORD32 u1_decode_nmb;
 dec_bit_stream_t * const ps_bitstrm = ps_dec->ps_bitstrm;
 dec_slice_params_t * ps_slice = ps_dec->ps_cur_slice;
    UWORD32 *pu4_bitstrm_buf = ps_bitstrm->pu4_buffer;
    UWORD32 *pu4_bitstrm_ofst = &ps_bitstrm->u4_ofst;
 deblk_mb_t *ps_cur_deblk_mb;
 dec_mb_info_t *ps_cur_mb_info;
 parse_pmbarams_t *ps_parse_mb_data;
    UWORD32 u1_inter_mb_type;
    UWORD32 u1_deblk_mb_type;
    UWORD16 u2_total_mbs_coded;
    UWORD32 u1_mbaff = ps_slice->u1_mbaff_frame_flag;
 parse_part_params_t *ps_part_info;
    WORD32 ret;


 if(ps_dec->ps_dec_err_status->u1_err_flag & REJECT_CUR_PIC)
 {

         ih264d_err_pic_dispbuf_mgr(ps_dec);
         return 0;
     }
    ps_dec->ps_dpb_cmds->u1_long_term_reference_flag = 0;
     if(prev_slice_err == 1)
     {
         /* first slice - missing/header corruption */
        ps_dec->ps_cur_slice->u2_frame_num = u2_frame_num;


 if(!ps_dec->u1_first_slice_in_stream)
 {
            ih264d_end_of_pic(ps_dec, u1_is_idr_slice,
                ps_dec->ps_cur_slice->u2_frame_num);
            ps_dec->s_cur_pic_poc.u2_frame_num =
                ps_dec->ps_cur_slice->u2_frame_num;
 }

 {
            WORD32 i, j, poc = 0;

            ps_dec->ps_cur_slice->u2_first_mb_in_slice = 0;

            ps_dec->pf_mvpred = ih264d_mvpred_nonmbaff;
            ps_dec->p_form_mb_part_info = ih264d_form_mb_part_info_bp;
            ps_dec->p_motion_compensate = ih264d_motion_compensate_bp;

 if(ps_dec->ps_cur_pic != NULL)
                poc = ps_dec->ps_cur_pic->i4_poc + 2;

            j = 0;
 for(i = 0; i < MAX_NUM_PIC_PARAMS; i++)
 if(ps_dec->ps_pps[i].u1_is_valid == TRUE)
                       j = i;
 {
                ps_dec->ps_cur_slice->u1_bottom_field_flag = 0;
                ps_dec->ps_cur_slice->u1_field_pic_flag = 0;
                ps_dec->ps_cur_slice->u1_slice_type = P_SLICE;
                ps_dec->ps_cur_slice->u1_nal_ref_idc = 1;
                ps_dec->ps_cur_slice->u1_nal_unit_type = 1;
                ret = ih264d_start_of_pic(ps_dec, poc, ps_cur_poc,
                        ps_dec->ps_cur_slice->u2_frame_num,
 &ps_dec->ps_pps[j]);

 if(ret != OK)
 {
 return ret;
 }
 }

            ps_dec->ps_ref_pic_buf_lx[0][0]->u1_pic_buf_id = 0;

            ps_dec->u4_output_present = 0;

 {
                ih264d_get_next_display_field(ps_dec,
                                              ps_dec->ps_out_buffer,
 &(ps_dec->s_disp_op));
 /* If error code is non-zero then there is no buffer available for display,
                 hence avoid format conversion */

 if(0 != ps_dec->s_disp_op.u4_error_code)
 {
                    ps_dec->u4_fmt_conv_cur_row = ps_dec->s_disp_frame_info.u4_y_ht;
 }
 else
                    ps_dec->u4_output_present = 1;
 }

 if(ps_dec->u1_separate_parse == 1)
 {
 if(ps_dec->u4_dec_thread_created == 0)
 {
                    ithread_create(ps_dec->pv_dec_thread_handle, NULL,
 (void *)ih264d_decode_picture_thread,
 (void *)ps_dec);

                    ps_dec->u4_dec_thread_created = 1;
 }

 if((ps_dec->u4_num_cores == 3) &&
 ((ps_dec->u4_app_disable_deblk_frm == 0) || ps_dec->i1_recon_in_thread3_flag)
 && (ps_dec->u4_bs_deblk_thread_created == 0))
 {
                    ps_dec->u4_start_recon_deblk = 0;
                    ithread_create(ps_dec->pv_bs_deblk_thread_handle, NULL,
 (void *)ih264d_recon_deblk_thread,
 (void *)ps_dec);
                    ps_dec->u4_bs_deblk_thread_created = 1;
 }
 }
 }
 }
 else
 {

 dec_slice_struct_t *ps_parse_cur_slice;
        ps_parse_cur_slice = ps_dec->ps_dec_slice_buf + ps_dec->u2_cur_slice_num;

 if(ps_dec->u1_slice_header_done
 && ps_parse_cur_slice == ps_dec->ps_parse_cur_slice)
 {
            u1_num_mbs = ps_dec->u4_num_mbs_cur_nmb;

 if(u1_num_mbs)
 {
                ps_cur_mb_info = ps_dec->ps_nmb_info + u1_num_mbs - 1;
 }
 else
 {
 if(ps_dec->u1_separate_parse)
 {
                    ps_cur_mb_info = ps_dec->ps_nmb_info - 1;
 }
 else
 {
                    ps_cur_mb_info = ps_dec->ps_nmb_info
 + ps_dec->u4_num_mbs_prev_nmb - 1;
 }
 }

            ps_dec->u2_mby = ps_cur_mb_info->u2_mby;
            ps_dec->u2_mbx = ps_cur_mb_info->u2_mbx;

            ps_dec->u1_mb_ngbr_availablity =
                    ps_cur_mb_info->u1_mb_ngbr_availablity;

            ps_dec->pv_parse_tu_coeff_data = ps_dec->pv_prev_mb_parse_tu_coeff_data;
            ps_dec->u2_cur_mb_addr--;
            ps_dec->i4_submb_ofst -= SUB_BLK_SIZE;

 if(u1_num_mbs)
 {
 if (ps_dec->u1_pr_sl_type == P_SLICE
 || ps_dec->u1_pr_sl_type == B_SLICE)
 {
                    ps_dec->pf_mvpred_ref_tfr_nby2mb(ps_dec, u1_mb_idx,    u1_num_mbs);
                    ps_dec->ps_part = ps_dec->ps_parse_part_params;
 }

                u1_num_mbs_next = i2_pic_wdin_mbs - ps_dec->u2_mbx - 1;
                u1_end_of_row = (!u1_num_mbs_next)
 && (!(u1_mbaff && (u1_num_mbs & 0x01)));
                u1_slice_end = 1;
                u1_tfr_n_mb = 1;
                ps_cur_mb_info->u1_end_of_slice = u1_slice_end;

 if(ps_dec->u1_separate_parse)
 {
                    ih264d_parse_tfr_nmb(ps_dec, u1_mb_idx, u1_num_mbs,
                            u1_num_mbs_next, u1_tfr_n_mb, u1_end_of_row);
                    ps_dec->ps_nmb_info += u1_num_mbs;
 }
 else
 {
                    ih264d_decode_recon_tfr_nmb(ps_dec, u1_mb_idx, u1_num_mbs,
                            u1_num_mbs_next, u1_tfr_n_mb, u1_end_of_row);
 }
                ps_dec->u2_total_mbs_coded += u1_num_mbs;
                ps_dec->u1_mb_idx = 0;
                ps_dec->u4_num_mbs_cur_nmb = 0;
 }

 if(ps_dec->u2_total_mbs_coded
 >= ps_dec->u2_frm_ht_in_mbs * ps_dec->u2_frm_wd_in_mbs)
 {
                ps_dec->u1_pic_decode_done = 1;
 return 0;
 }

            ps_dec->u2_cur_slice_num++;
             ps_dec->i2_prev_slice_mbx = ps_dec->u2_mbx;
            ps_dec->i2_prev_slice_mby = ps_dec->u2_mby;
            ps_dec->ps_parse_cur_slice++;

 }
 else
 {
            ps_dec->ps_parse_cur_slice = ps_dec->ps_dec_slice_buf
 + ps_dec->u2_cur_slice_num;
 }
 }

 /******************************************************/
 /* Initializations to new slice                       */
 /******************************************************/
 {
        WORD32 num_entries;
        WORD32 size;
        UWORD8 *pu1_buf;

        num_entries = MAX_FRAMES;
 if((1 >= ps_dec->ps_cur_sps->u1_num_ref_frames) &&
 (0 == ps_dec->i4_display_delay))
 {
            num_entries = 1;
 }
        num_entries = ((2 * num_entries) + 1);
 if(BASE_PROFILE_IDC != ps_dec->ps_cur_sps->u1_profile_idc)
 {
            num_entries *= 2;
 }
        size = num_entries * sizeof(void *);
        size += PAD_MAP_IDX_POC * sizeof(void *);

        pu1_buf = (UWORD8 *)ps_dec->pv_map_ref_idx_to_poc_buf;
        pu1_buf += size * ps_dec->u2_cur_slice_num;
        ps_dec->ps_parse_cur_slice->ppv_map_ref_idx_to_poc = (volatile void **)pu1_buf;
 }

    ps_dec->ps_cur_slice->u2_first_mb_in_slice = ps_dec->u2_total_mbs_coded << u1_mbaff;
    ps_dec->ps_cur_slice->i1_slice_alpha_c0_offset = 0;
    ps_dec->ps_cur_slice->i1_slice_beta_offset = 0;

 if(ps_dec->ps_cur_slice->u1_field_pic_flag)
        ps_dec->u2_prv_frame_num = ps_dec->ps_cur_slice->u2_frame_num;

    ps_dec->ps_parse_cur_slice->u4_first_mb_in_slice = ps_dec->u2_total_mbs_coded << u1_mbaff;
    ps_dec->ps_parse_cur_slice->u2_log2Y_crwd =    ps_dec->ps_cur_slice->u2_log2Y_crwd;


 if(ps_dec->u1_separate_parse)
 {
        ps_dec->ps_parse_cur_slice->pv_tu_coeff_data_start = ps_dec->pv_parse_tu_coeff_data;
 }
 else
 {
        ps_dec->pv_proc_tu_coeff_data = ps_dec->pv_parse_tu_coeff_data;
 }

 /******************************************************/
 /* Initializations specific to P slice                */
 /******************************************************/
    u1_inter_mb_type = P_MB;
    u1_deblk_mb_type = D_INTER_MB;

    ps_dec->ps_cur_slice->u1_slice_type = P_SLICE;
    ps_dec->ps_parse_cur_slice->slice_type = P_SLICE;
    ps_dec->pf_mvpred_ref_tfr_nby2mb = ih264d_mv_pred_ref_tfr_nby2_pmb;
    ps_dec->ps_part = ps_dec->ps_parse_part_params;

 /******************************************************/
 /* Parsing / decoding the slice                       */
 /******************************************************/
    ps_dec->u1_slice_header_done = 2;
    ps_dec->u1_qp = ps_slice->u1_slice_qp;
    ih264d_update_qp(ps_dec, 0);
    u1_mb_idx = ps_dec->u1_mb_idx;
    ps_parse_mb_data = ps_dec->ps_parse_mb_data;
    u1_num_mbs = u1_mb_idx;

    u1_slice_end = 0;
    u1_tfr_n_mb = 0;
    u1_decode_nmb = 0;
    u1_num_mbsNby2 = 0;
    i2_cur_mb_addr = ps_dec->u2_total_mbs_coded;
    i2_mb_skip_run = num_mb_skip;

 while(!u1_slice_end)
 {
        UWORD8 u1_mb_type;

 if(i2_cur_mb_addr > ps_dec->ps_cur_sps->u2_max_mb_addr)
 break;

        ps_cur_mb_info = ps_dec->ps_nmb_info + u1_num_mbs;
        ps_dec->u4_num_mbs_cur_nmb = u1_num_mbs;

        ps_cur_mb_info->u1_Mux = 0;
        ps_dec->u4_num_pmbair = (u1_num_mbs >> u1_mbaff);
        ps_cur_deblk_mb = ps_dec->ps_deblk_mbn + u1_num_mbs;

        ps_cur_mb_info->u1_end_of_slice = 0;

 /* Storing Default partition info */
        ps_parse_mb_data->u1_num_part = 1;
        ps_parse_mb_data->u1_isI_mb = 0;

 /**************************************************************/
 /* Get the required information for decoding of MB            */
 /**************************************************************/
 /* mb_x, mb_y, neighbor availablity, */
 if (u1_mbaff)
            ih264d_get_mb_info_cavlc_mbaff(ps_dec, i2_cur_mb_addr, ps_cur_mb_info, i2_mb_skip_run);
 else
            ih264d_get_mb_info_cavlc_nonmbaff(ps_dec, i2_cur_mb_addr, ps_cur_mb_info, i2_mb_skip_run);

 /* Set the deblocking parameters for this MB */
 if(ps_dec->u4_app_disable_deblk_frm == 0)
 {
            ih264d_set_deblocking_parameters(ps_cur_deblk_mb, ps_slice,
                                             ps_dec->u1_mb_ngbr_availablity,
                                             ps_dec->u1_cur_mb_fld_dec_flag);
 }

 /* Set appropriate flags in ps_cur_mb_info and ps_dec */
        ps_dec->i1_prev_mb_qp_delta = 0;
        ps_dec->u1_sub_mb_num = 0;
        ps_cur_mb_info->u1_mb_type = MB_SKIP;
        ps_cur_mb_info->u1_mb_mc_mode = PRED_16x16;
        ps_cur_mb_info->u1_cbp = 0;

 /* Storing Skip partition info */
        ps_part_info = ps_dec->ps_part;
        ps_part_info->u1_is_direct = PART_DIRECT_16x16;
        ps_part_info->u1_sub_mb_num = 0;
        ps_dec->ps_part++;

 /* Update Nnzs */
        ih264d_update_nnz_for_skipmb(ps_dec, ps_cur_mb_info, CAVLC);

        ps_cur_mb_info->ps_curmb->u1_mb_type = u1_inter_mb_type;
        ps_cur_deblk_mb->u1_mb_type |= u1_deblk_mb_type;

        i2_mb_skip_run--;

        ps_cur_deblk_mb->u1_mb_qp = ps_dec->u1_qp;

 if (u1_mbaff)
 {
            ih264d_update_mbaff_left_nnz(ps_dec, ps_cur_mb_info);
 }

 /**************************************************************/
 /* Get next Macroblock address                                */
 /**************************************************************/
        i2_cur_mb_addr++;

        u1_num_mbs++;
        u1_num_mbsNby2++;
        ps_parse_mb_data++;

 /****************************************************************/
 /* Check for End Of Row and other flags that determine when to  */
 /* do DMA setup for N/2-Mb, Decode for N-Mb, and Transfer for   */
 /* N-Mb                                                         */
 /****************************************************************/
        u1_num_mbs_next = i2_pic_wdin_mbs - ps_dec->u2_mbx - 1;
        u1_end_of_row = (!u1_num_mbs_next) && (!(u1_mbaff && (u1_num_mbs & 0x01)));
        u1_slice_end = !i2_mb_skip_run;
        u1_tfr_n_mb = (u1_num_mbs == ps_dec->u1_recon_mb_grp) || u1_end_of_row
 || u1_slice_end;
        u1_decode_nmb = u1_tfr_n_mb || u1_slice_end;
        ps_cur_mb_info->u1_end_of_slice = u1_slice_end;

 if(u1_decode_nmb)
 {
            ps_dec->pf_mvpred_ref_tfr_nby2mb(ps_dec, u1_mb_idx, u1_num_mbs);
            u1_num_mbsNby2 = 0;

            ps_parse_mb_data = ps_dec->ps_parse_mb_data;
            ps_dec->ps_part = ps_dec->ps_parse_part_params;

 if(ps_dec->u1_separate_parse)
 {
                ih264d_parse_tfr_nmb(ps_dec, u1_mb_idx, u1_num_mbs,
                                     u1_num_mbs_next, u1_tfr_n_mb, u1_end_of_row);
                ps_dec->ps_nmb_info +=  u1_num_mbs;
 }
 else
 {
                ih264d_decode_recon_tfr_nmb(ps_dec, u1_mb_idx, u1_num_mbs, u1_num_mbs_next,
                                            u1_tfr_n_mb, u1_end_of_row);
 }
            ps_dec->u2_total_mbs_coded += u1_num_mbs;
 if(u1_tfr_n_mb)
                u1_num_mbs = 0;
            u1_mb_idx = u1_num_mbs;
            ps_dec->u1_mb_idx = u1_num_mbs;
 }
 }

    ps_dec->u4_num_mbs_cur_nmb = 0;
    ps_dec->ps_cur_slice->u4_mbs_in_slice = i2_cur_mb_addr
 - ps_dec->ps_parse_cur_slice->u4_first_mb_in_slice;

    H264_DEC_DEBUG_PRINT("Mbs in slice: %d\n", ps_dec->ps_cur_slice->u4_mbs_in_slice);

    ps_dec->u2_cur_slice_num++;

 /* incremented here only if first slice is inserted */
 if(ps_dec->u4_first_slice_in_pic != 0)
        ps_dec->ps_parse_cur_slice++;

    ps_dec->i2_prev_slice_mbx = ps_dec->u2_mbx;
    ps_dec->i2_prev_slice_mby = ps_dec->u2_mby;

 if(ps_dec->u2_total_mbs_coded
 >= ps_dec->u2_frm_ht_in_mbs * ps_dec->u2_frm_wd_in_mbs)
 {
        ps_dec->u1_pic_decode_done = 1;
 }

 return 0;

}
