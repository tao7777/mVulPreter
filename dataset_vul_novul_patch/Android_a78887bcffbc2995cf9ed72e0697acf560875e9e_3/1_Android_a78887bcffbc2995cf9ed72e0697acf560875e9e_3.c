WORD32 ih264d_start_of_pic(dec_struct_t *ps_dec,
                         WORD32 i4_poc,
 pocstruct_t *ps_temp_poc,
                         UWORD16 u2_frame_num,
 dec_pic_params_t *ps_pps)
{
 pocstruct_t *ps_prev_poc = &ps_dec->s_cur_pic_poc;
 pocstruct_t *ps_cur_poc = ps_temp_poc;

 pic_buffer_t *pic_buf;

 ivd_video_decode_op_t * ps_dec_output =
 (ivd_video_decode_op_t *)ps_dec->pv_dec_out;
 dec_slice_params_t *ps_cur_slice = ps_dec->ps_cur_slice;
 dec_seq_params_t *ps_seq = ps_pps->ps_sps;
    UWORD8 u1_bottom_field_flag = ps_cur_slice->u1_bottom_field_flag;
    UWORD8 u1_field_pic_flag = ps_cur_slice->u1_field_pic_flag;
 /* high profile related declarations */
 high_profile_tools_t s_high_profile;
    WORD32 ret;

    H264_MUTEX_LOCK(&ps_dec->process_disp_mutex);

    ps_prev_poc->i4_pic_order_cnt_lsb = ps_cur_poc->i4_pic_order_cnt_lsb;
    ps_prev_poc->i4_pic_order_cnt_msb = ps_cur_poc->i4_pic_order_cnt_msb;
    ps_prev_poc->i4_delta_pic_order_cnt_bottom =
                    ps_cur_poc->i4_delta_pic_order_cnt_bottom;
    ps_prev_poc->i4_delta_pic_order_cnt[0] =
                    ps_cur_poc->i4_delta_pic_order_cnt[0];
    ps_prev_poc->i4_delta_pic_order_cnt[1] =
                    ps_cur_poc->i4_delta_pic_order_cnt[1];
    ps_prev_poc->u1_bot_field = ps_dec->ps_cur_slice->u1_bottom_field_flag;
    ps_prev_poc->i4_prev_frame_num_ofst = ps_cur_poc->i4_prev_frame_num_ofst;
    ps_prev_poc->u2_frame_num = u2_frame_num;
    ps_dec->i1_prev_mb_qp_delta = 0;
    ps_dec->i1_next_ctxt_idx = 0;


    ps_dec->u4_nmb_deblk = 0;
 if(ps_dec->u4_num_cores == 1)
       ps_dec->u4_nmb_deblk = 1;



 if(ps_seq->u1_mb_aff_flag == 1)
 {
        ps_dec->u4_nmb_deblk = 0;
 if(ps_dec->u4_num_cores > 2)
            ps_dec->u4_num_cores = 2;
 }

        ps_dec->u4_use_intrapred_line_copy = 0;



 if (ps_seq->u1_mb_aff_flag == 0)
 {
        ps_dec->u4_use_intrapred_line_copy = 1;
 }

    ps_dec->u4_app_disable_deblk_frm = 0;
 /* If degrade is enabled, set the degrade flags appropriately */
 if(ps_dec->i4_degrade_type && ps_dec->i4_degrade_pics)
 {
        WORD32 degrade_pic;
        ps_dec->i4_degrade_pic_cnt++;
        degrade_pic = 0;

 /* If degrade is to be done in all frames, then do not check further */
 switch(ps_dec->i4_degrade_pics)
 {
 case 4:
 {
                degrade_pic = 1;
 break;
 }
 case 3:
 {
 if(ps_cur_slice->u1_slice_type != I_SLICE)
                    degrade_pic = 1;

 break;
 }
 case 2:
 {

 /* If pic count hits non-degrade interval or it is an islice, then do not degrade */
 if((ps_cur_slice->u1_slice_type != I_SLICE)
 && (ps_dec->i4_degrade_pic_cnt
 != ps_dec->i4_nondegrade_interval))
                    degrade_pic = 1;

 break;
 }
 case 1:
 {
 /* Check if the current picture is non-ref */
 if(0 == ps_cur_slice->u1_nal_ref_idc)
 {
                    degrade_pic = 1;
 }
 break;
 }

 }
 if(degrade_pic)
 {
 if(ps_dec->i4_degrade_type & 0x2)
                ps_dec->u4_app_disable_deblk_frm = 1;

 /* MC degrading is done only for non-ref pictures */
 if(0 == ps_cur_slice->u1_nal_ref_idc)
 {
 if(ps_dec->i4_degrade_type & 0x4)
                    ps_dec->i4_mv_frac_mask = 0;

 if(ps_dec->i4_degrade_type & 0x8)
                    ps_dec->i4_mv_frac_mask = 0;
 }
 }
 else
            ps_dec->i4_degrade_pic_cnt = 0;
 }

 {
 dec_err_status_t * ps_err = ps_dec->ps_dec_err_status;
 if(ps_dec->u1_sl_typ_5_9
 && ((ps_cur_slice->u1_slice_type == I_SLICE)
 || (ps_cur_slice->u1_slice_type
 == SI_SLICE)))
            ps_err->u1_cur_pic_type = PIC_TYPE_I;
 else
            ps_err->u1_cur_pic_type = PIC_TYPE_UNKNOWN;

 if(ps_err->u1_pic_aud_i == PIC_TYPE_I)
 {
            ps_err->u1_cur_pic_type = PIC_TYPE_I;
            ps_err->u1_pic_aud_i = PIC_TYPE_UNKNOWN;
 }

 if(ps_cur_slice->u1_nal_unit_type == IDR_SLICE_NAL)
 {
 if(ps_err->u1_err_flag)
                ih264d_reset_ref_bufs(ps_dec->ps_dpb_mgr);
            ps_err->u1_err_flag = ACCEPT_ALL_PICS;
 }
 }

 if(ps_dec->u1_init_dec_flag && ps_dec->s_prev_seq_params.u1_eoseq_pending)
 {
 /* Reset the decoder picture buffers */
        WORD32 j;
 for(j = 0; j < MAX_DISP_BUFS_NEW; j++)
 {

            ih264_buf_mgr_release((buf_mgr_t *)ps_dec->pv_pic_buf_mgr,
                                  j,
                                  BUF_MGR_REF);
            ih264_buf_mgr_release((buf_mgr_t *)ps_dec->pv_mv_buf_mgr,
                                  ps_dec->au1_pic_buf_id_mv_buf_id_map[j],
                                  BUF_MGR_REF);
            ih264_buf_mgr_release((buf_mgr_t *)ps_dec->pv_pic_buf_mgr,
                                  j,
                                  BUF_MGR_IO);
 }

 /* reset the decoder structure parameters related to buffer handling */
        ps_dec->u1_second_field = 0;
        ps_dec->i4_cur_display_seq = 0;

 /********************************************************************/
 /* indicate in the decoder output i4_status that some frames are being */
 /* dropped, so that it resets timestamp and wait for a new sequence */
 /********************************************************************/

        ps_dec->s_prev_seq_params.u1_eoseq_pending = 0;
 }
    ret = ih264d_init_pic(ps_dec, u2_frame_num, i4_poc, ps_pps);
 if(ret != OK)
 return ret;

    ps_dec->pv_parse_tu_coeff_data = ps_dec->pv_pic_tu_coeff_data;
    ps_dec->pv_proc_tu_coeff_data  = ps_dec->pv_pic_tu_coeff_data;
    ps_dec->ps_nmb_info = ps_dec->ps_frm_mb_info;
 if(ps_dec->u1_separate_parse)
 {
        UWORD16 pic_wd;
        UWORD16 pic_ht;
        UWORD32 num_mbs;

        pic_wd = ps_dec->u2_pic_wd;
        pic_ht = ps_dec->u2_pic_ht;
        num_mbs = (pic_wd * pic_ht) >> 8;

 if(ps_dec->pu1_dec_mb_map)
 {
            memset((void *)ps_dec->pu1_dec_mb_map, 0, num_mbs);
 }

 if(ps_dec->pu1_recon_mb_map)
 {

            memset((void *)ps_dec->pu1_recon_mb_map, 0, num_mbs);
 }

 if(ps_dec->pu2_slice_num_map)
 {
            memset((void *)ps_dec->pu2_slice_num_map, 0,
 (num_mbs * sizeof(UWORD16)));
 }

 }


     ps_dec->ps_parse_cur_slice = &(ps_dec->ps_dec_slice_buf[0]);
     ps_dec->ps_decode_cur_slice = &(ps_dec->ps_dec_slice_buf[0]);
     ps_dec->ps_computebs_cur_slice = &(ps_dec->ps_dec_slice_buf[0]);
 
     /* Initialize all the HP toolsets to zero */
     ps_dec->s_high_profile.u1_scaling_present = 0;
    ps_dec->s_high_profile.u1_transform8x8_present = 0;

 /* Get Next Free Picture */
 if(1 == ps_dec->u4_share_disp_buf)
 {
        UWORD32 i;
 /* Free any buffer that is in the queue to be freed */
 for(i = 0; i < MAX_DISP_BUFS_NEW; i++)
 {
 if(0 == ps_dec->u4_disp_buf_to_be_freed[i])
 continue;
            ih264_buf_mgr_release((buf_mgr_t *)ps_dec->pv_pic_buf_mgr, i,
            BUF_MGR_IO);
            ps_dec->u4_disp_buf_to_be_freed[i] = 0;
            ps_dec->u4_disp_buf_mapping[i] = 0;

 }
 }
 if(!(u1_field_pic_flag && 0 != ps_dec->u1_top_bottom_decoded)) //ps_dec->u1_second_field))
 {
 pic_buffer_t *ps_cur_pic;
        WORD32 cur_pic_buf_id, cur_mv_buf_id;
 col_mv_buf_t *ps_col_mv;
 while(1)
 {
            ps_cur_pic = (pic_buffer_t *)ih264_buf_mgr_get_next_free(
 (buf_mgr_t *)ps_dec->pv_pic_buf_mgr,
 &cur_pic_buf_id);
 if(ps_cur_pic == NULL)
 {
                ps_dec->i4_error_code = ERROR_UNAVAIL_PICBUF_T;
 return ERROR_UNAVAIL_PICBUF_T;
 }
 if(0 == ps_dec->u4_disp_buf_mapping[cur_pic_buf_id])
 {
 break;
 }

 }
        ps_col_mv = (col_mv_buf_t *)ih264_buf_mgr_get_next_free((buf_mgr_t *)ps_dec->pv_mv_buf_mgr,
 &cur_mv_buf_id);
 if(ps_col_mv == NULL)
 {
            ps_dec->i4_error_code = ERROR_UNAVAIL_MVBUF_T;
 return ERROR_UNAVAIL_MVBUF_T;
 }

        ps_dec->ps_cur_pic = ps_cur_pic;
        ps_dec->u1_pic_buf_id = cur_pic_buf_id;
        ps_cur_pic->u4_ts = ps_dec->u4_ts;


        ps_cur_pic->u1_mv_buf_id = cur_mv_buf_id;
        ps_dec->au1_pic_buf_id_mv_buf_id_map[cur_pic_buf_id] = cur_mv_buf_id;

        ps_cur_pic->pu1_col_zero_flag = (UWORD8 *)ps_col_mv->pv_col_zero_flag;
        ps_cur_pic->ps_mv = (mv_pred_t *)ps_col_mv->pv_mv;
        ps_dec->au1_pic_buf_ref_flag[cur_pic_buf_id] = 0;
 if(ps_dec->u1_first_slice_in_stream)
 {
 /*make first entry of list0 point to cur pic,so that if first Islice is in error, ref pic struct will have valid entries*/
            ps_dec->ps_ref_pic_buf_lx[0] = ps_dec->ps_dpb_mgr->ps_init_dpb[0];
 *(ps_dec->ps_dpb_mgr->ps_init_dpb[0][0]) = *ps_cur_pic;
 }

 if(!ps_dec->ps_cur_pic)
 {
            WORD32 j;
            H264_DEC_DEBUG_PRINT("------- Display Buffers Reset --------\n");
 for(j = 0; j < MAX_DISP_BUFS_NEW; j++)
 {

                ih264_buf_mgr_release((buf_mgr_t *)ps_dec->pv_pic_buf_mgr,
                                      j,
                                      BUF_MGR_REF);
                ih264_buf_mgr_release((buf_mgr_t *)ps_dec->pv_mv_buf_mgr,
                                      ps_dec->au1_pic_buf_id_mv_buf_id_map[j],
                                      BUF_MGR_REF);
                ih264_buf_mgr_release((buf_mgr_t *)ps_dec->pv_pic_buf_mgr,
                                      j,
                                      BUF_MGR_IO);
 }

            ps_dec->i4_cur_display_seq = 0;
            ps_dec->i4_prev_max_display_seq = 0;
            ps_dec->i4_max_poc = 0;

            ps_cur_pic = (pic_buffer_t *)ih264_buf_mgr_get_next_free(
 (buf_mgr_t *)ps_dec->pv_pic_buf_mgr,
 &cur_pic_buf_id);
 if(ps_cur_pic == NULL)
 {
                ps_dec->i4_error_code = ERROR_UNAVAIL_PICBUF_T;
 return ERROR_UNAVAIL_PICBUF_T;
 }

            ps_col_mv = (col_mv_buf_t *)ih264_buf_mgr_get_next_free((buf_mgr_t *)ps_dec->pv_mv_buf_mgr,
 &cur_mv_buf_id);
 if(ps_col_mv == NULL)
 {
                ps_dec->i4_error_code = ERROR_UNAVAIL_MVBUF_T;
 return ERROR_UNAVAIL_MVBUF_T;
 }

            ps_dec->ps_cur_pic = ps_cur_pic;
            ps_dec->u1_pic_buf_id = cur_pic_buf_id;
            ps_cur_pic->u4_ts = ps_dec->u4_ts;
            ps_dec->apv_buf_id_pic_buf_map[cur_pic_buf_id] = (void *)ps_cur_pic;

            ps_cur_pic->u1_mv_buf_id = cur_mv_buf_id;
            ps_dec->au1_pic_buf_id_mv_buf_id_map[cur_pic_buf_id] = cur_mv_buf_id;

            ps_cur_pic->pu1_col_zero_flag = (UWORD8 *)ps_col_mv->pv_col_zero_flag;
            ps_cur_pic->ps_mv = (mv_pred_t *)ps_col_mv->pv_mv;
            ps_dec->au1_pic_buf_ref_flag[cur_pic_buf_id] = 0;

 }

        ps_dec->ps_cur_pic->u1_picturetype = u1_field_pic_flag;
        ps_dec->ps_cur_pic->u4_pack_slc_typ = SKIP_NONE;
        H264_DEC_DEBUG_PRINT("got a buffer\n");
 }
 else
 {
        H264_DEC_DEBUG_PRINT("did not get a buffer\n");
 }

    ps_dec->u4_pic_buf_got = 1;

    ps_dec->ps_cur_pic->i4_poc = i4_poc;
    ps_dec->ps_cur_pic->i4_frame_num = u2_frame_num;
    ps_dec->ps_cur_pic->i4_pic_num = u2_frame_num;
    ps_dec->ps_cur_pic->i4_top_field_order_cnt = ps_pps->i4_top_field_order_cnt;
    ps_dec->ps_cur_pic->i4_bottom_field_order_cnt =
                    ps_pps->i4_bottom_field_order_cnt;
    ps_dec->ps_cur_pic->i4_avg_poc = ps_pps->i4_avg_poc;
    ps_dec->ps_cur_pic->u4_time_stamp = ps_dec->u4_pts;

    ps_dec->s_cur_pic = *(ps_dec->ps_cur_pic);
 if(u1_field_pic_flag && u1_bottom_field_flag)
 {
        WORD32 i4_temp_poc;
        WORD32 i4_top_field_order_poc, i4_bot_field_order_poc;
 /* Point to odd lines, since it's bottom field */
        ps_dec->s_cur_pic.pu1_buf1 += ps_dec->s_cur_pic.u2_frm_wd_y;
        ps_dec->s_cur_pic.pu1_buf2 += ps_dec->s_cur_pic.u2_frm_wd_uv;
        ps_dec->s_cur_pic.pu1_buf3 += ps_dec->s_cur_pic.u2_frm_wd_uv;
        ps_dec->s_cur_pic.ps_mv +=
 ((ps_dec->u2_pic_ht * ps_dec->u2_pic_wd) >> 5);
        ps_dec->s_cur_pic.pu1_col_zero_flag += ((ps_dec->u2_pic_ht
 * ps_dec->u2_pic_wd) >> 5);
        ps_dec->ps_cur_pic->u1_picturetype |= BOT_FLD;
        i4_top_field_order_poc = ps_dec->ps_cur_pic->i4_top_field_order_cnt;
        i4_bot_field_order_poc = ps_dec->ps_cur_pic->i4_bottom_field_order_cnt;
        i4_temp_poc = MIN(i4_top_field_order_poc,
                                 i4_bot_field_order_poc);
        ps_dec->ps_cur_pic->i4_avg_poc = i4_temp_poc;
 }

    ps_cur_slice->u1_mbaff_frame_flag = ps_seq->u1_mb_aff_flag
 && (!u1_field_pic_flag);

    ps_dec->ps_cur_pic->u1_picturetype |= (ps_cur_slice->u1_mbaff_frame_flag
 << 2);

    ps_dec->ps_cur_mb_row = ps_dec->ps_nbr_mb_row; //[0];
    ps_dec->ps_cur_mb_row += 2;
    ps_dec->ps_top_mb_row = ps_dec->ps_nbr_mb_row;
    ps_dec->ps_top_mb_row += ((ps_dec->u2_frm_wd_in_mbs + 2) << (1 - ps_dec->ps_cur_sps->u1_frame_mbs_only_flag));
    ps_dec->ps_top_mb_row += 2;

 /* CHANGED CODE */
    ps_dec->ps_mv_cur = ps_dec->s_cur_pic.ps_mv;
    ps_dec->ps_mv_top = ps_dec->ps_mv_top_p[0];
 /* CHANGED CODE */
    ps_dec->u1_mv_top_p = 0;
    ps_dec->u1_mb_idx = 0;
 /* CHANGED CODE */
    ps_dec->ps_mv_left = ps_dec->s_cur_pic.ps_mv;
    ps_dec->u2_total_mbs_coded = 0;
    ps_dec->i4_submb_ofst = -(SUB_BLK_SIZE);
    ps_dec->u4_pred_info_idx = 0;
    ps_dec->u4_pred_info_pkd_idx = 0;
    ps_dec->u4_dma_buf_idx = 0;
    ps_dec->ps_mv = ps_dec->s_cur_pic.ps_mv;
    ps_dec->ps_mv_bank_cur = ps_dec->s_cur_pic.ps_mv;
    ps_dec->pu1_col_zero_flag = ps_dec->s_cur_pic.pu1_col_zero_flag;
    ps_dec->ps_part = ps_dec->ps_parse_part_params;
    ps_dec->i2_prev_slice_mbx = -1;
    ps_dec->i2_prev_slice_mby = 0;
    ps_dec->u2_mv_2mb[0] = 0;

     ps_dec->u2_mv_2mb[1] = 0;
     ps_dec->u1_last_pic_not_decoded = 0;
 
    ps_dec->u2_cur_slice_num = 0;
     ps_dec->u2_cur_slice_num_dec_thread = 0;
     ps_dec->u2_cur_slice_num_bs = 0;
     ps_dec->u4_intra_pred_line_ofst = 0;
    ps_dec->pu1_cur_y_intra_pred_line = ps_dec->pu1_y_intra_pred_line;
    ps_dec->pu1_cur_u_intra_pred_line = ps_dec->pu1_u_intra_pred_line;
    ps_dec->pu1_cur_v_intra_pred_line = ps_dec->pu1_v_intra_pred_line;

    ps_dec->pu1_cur_y_intra_pred_line_base = ps_dec->pu1_y_intra_pred_line;
    ps_dec->pu1_cur_u_intra_pred_line_base = ps_dec->pu1_u_intra_pred_line;
    ps_dec->pu1_cur_v_intra_pred_line_base = ps_dec->pu1_v_intra_pred_line;





    ps_dec->pu1_prev_y_intra_pred_line = ps_dec->pu1_y_intra_pred_line
 + (ps_dec->u2_frm_wd_in_mbs * MB_SIZE);

    ps_dec->pu1_prev_u_intra_pred_line = ps_dec->pu1_u_intra_pred_line
 + ps_dec->u2_frm_wd_in_mbs * BLK8x8SIZE * YUV420SP_FACTOR;
    ps_dec->pu1_prev_v_intra_pred_line = ps_dec->pu1_v_intra_pred_line
 + ps_dec->u2_frm_wd_in_mbs * BLK8x8SIZE;

    ps_dec->ps_deblk_mbn = ps_dec->ps_deblk_pic;
 /* Initialize The Function Pointer Depending Upon the Entropy and MbAff Flag */
 {
 if(ps_cur_slice->u1_mbaff_frame_flag)
 {
            ps_dec->pf_compute_bs = ih264d_compute_bs_mbaff;
            ps_dec->pf_mvpred = ih264d_mvpred_mbaff;
 }
 else
 {
            ps_dec->pf_compute_bs = ih264d_compute_bs_non_mbaff;
            ps_dec->u1_cur_mb_fld_dec_flag = ps_cur_slice->u1_field_pic_flag;
 }
 }
 /* Set up the Parameter for DMA transfer */
 {
        UWORD8 u1_field_pic_flag = ps_dec->ps_cur_slice->u1_field_pic_flag;

        UWORD8 u1_mbaff = ps_cur_slice->u1_mbaff_frame_flag;

        UWORD8 uc_lastmbs = (((ps_dec->u2_pic_wd) >> 4)
 % (ps_dec->u1_recon_mb_grp >> u1_mbaff));
        UWORD16 ui16_lastmbs_widthY =
 (uc_lastmbs ? (uc_lastmbs << 4) : ((ps_dec->u1_recon_mb_grp
 >> u1_mbaff) << 4));
        UWORD16 ui16_lastmbs_widthUV =
                        uc_lastmbs ? (uc_lastmbs << 3) : ((ps_dec->u1_recon_mb_grp
 >> u1_mbaff) << 3);

        ps_dec->s_tran_addrecon.pu1_dest_y = ps_dec->s_cur_pic.pu1_buf1;
        ps_dec->s_tran_addrecon.pu1_dest_u = ps_dec->s_cur_pic.pu1_buf2;
        ps_dec->s_tran_addrecon.pu1_dest_v = ps_dec->s_cur_pic.pu1_buf3;

        ps_dec->s_tran_addrecon.u2_frm_wd_y = ps_dec->u2_frm_wd_y
 << u1_field_pic_flag;
        ps_dec->s_tran_addrecon.u2_frm_wd_uv = ps_dec->u2_frm_wd_uv
 << u1_field_pic_flag;

 if(u1_field_pic_flag)
 {
            ui16_lastmbs_widthY += ps_dec->u2_frm_wd_y;
            ui16_lastmbs_widthUV += ps_dec->u2_frm_wd_uv;
 }

 /* Normal Increment of Pointer */
        ps_dec->s_tran_addrecon.u4_inc_y[0] = ((ps_dec->u1_recon_mb_grp << 4)
 >> u1_mbaff);
        ps_dec->s_tran_addrecon.u4_inc_uv[0] = ((ps_dec->u1_recon_mb_grp << 4)
 >> u1_mbaff);

 /* End of Row Increment */
        ps_dec->s_tran_addrecon.u4_inc_y[1] = (ui16_lastmbs_widthY
 + (PAD_LEN_Y_H << 1)
 + ps_dec->s_tran_addrecon.u2_frm_wd_y
 * ((15 << u1_mbaff) + u1_mbaff));
        ps_dec->s_tran_addrecon.u4_inc_uv[1] = (ui16_lastmbs_widthUV
 + (PAD_LEN_UV_H << 2)
 + ps_dec->s_tran_addrecon.u2_frm_wd_uv
 * ((15 << u1_mbaff) + u1_mbaff));

 /* Assign picture numbers to each frame/field  */
 /* only once per picture.                      */
        ih264d_assign_pic_num(ps_dec);
        ps_dec->s_tran_addrecon.u2_mv_top_left_inc = (ps_dec->u1_recon_mb_grp
 << 2) - 1 - (u1_mbaff << 2);
        ps_dec->s_tran_addrecon.u2_mv_left_inc = ((ps_dec->u1_recon_mb_grp
 >> u1_mbaff) - 1) << (4 + u1_mbaff);
 }
 /**********************************************************************/
 /* High profile related initialization at pictrue level               */
 /**********************************************************************/
 if(ps_seq->u1_profile_idc == HIGH_PROFILE_IDC)
 {
 if((ps_seq->i4_seq_scaling_matrix_present_flag)
 || (ps_pps->i4_pic_scaling_matrix_present_flag))
 {
            ih264d_form_scaling_matrix_picture(ps_seq, ps_pps, ps_dec);
            ps_dec->s_high_profile.u1_scaling_present = 1;
 }
 else
 {
            ih264d_form_default_scaling_matrix(ps_dec);
 }

 if(ps_pps->i4_transform_8x8_mode_flag)
 {
            ps_dec->s_high_profile.u1_transform8x8_present = 1;
 }
 }
 else
 {
        ih264d_form_default_scaling_matrix(ps_dec);
 }

 /* required while reading the transform_size_8x8 u4_flag */
    ps_dec->s_high_profile.u1_direct_8x8_inference_flag =
                    ps_seq->u1_direct_8x8_inference_flag;
    ps_dec->s_high_profile.s_cavlc_ctxt = ps_dec->s_cavlc_ctxt;

    ps_dec->i1_recon_in_thread3_flag = 1;
    ps_dec->ps_frame_buf_ip_recon = &ps_dec->s_tran_addrecon;
 if(ps_dec->u1_separate_parse)
 {
        memcpy(&ps_dec->s_tran_addrecon_parse, &ps_dec->s_tran_addrecon,
 sizeof(tfr_ctxt_t));
 if(ps_dec->u4_num_cores >= 3 && ps_dec->i1_recon_in_thread3_flag)
 {
            memcpy(&ps_dec->s_tran_iprecon, &ps_dec->s_tran_addrecon,
 sizeof(tfr_ctxt_t));
            ps_dec->ps_frame_buf_ip_recon = &ps_dec->s_tran_iprecon;
 }
 }


    ih264d_init_deblk_tfr_ctxt(ps_dec,&(ps_dec->s_pad_mgr), &(ps_dec->s_tran_addrecon),
                               ps_dec->u2_frm_wd_in_mbs, 0);

    ps_dec->ps_cur_deblk_mb = ps_dec->ps_deblk_pic;
    ps_dec->u4_cur_deblk_mb_num = 0;

    ps_dec->u4_deblk_mb_x = 0;
    ps_dec->u4_deblk_mb_y = 0;
    ps_dec->pu4_wt_ofsts = ps_dec->pu4_wts_ofsts_mat;

    H264_MUTEX_UNLOCK(&ps_dec->process_disp_mutex);
 return OK;
}
