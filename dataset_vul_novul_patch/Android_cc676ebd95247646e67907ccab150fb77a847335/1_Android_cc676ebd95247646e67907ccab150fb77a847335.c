void ih264d_init_decoder(void * ps_dec_params)
{
 dec_struct_t * ps_dec = (dec_struct_t *)ps_dec_params;
 dec_slice_params_t *ps_cur_slice;
 pocstruct_t *ps_prev_poc, *ps_cur_poc;
    WORD32 size;

    size = sizeof(pred_info_t) * 2 * 32;
    memset(ps_dec->ps_pred, 0 , size);

    size = sizeof(disp_mgr_t);
    memset(ps_dec->pv_disp_buf_mgr, 0 , size);

    size = sizeof(buf_mgr_t) + ithread_get_mutex_lock_size();
    memset(ps_dec->pv_pic_buf_mgr, 0, size);

    size = sizeof(dec_err_status_t);
    memset(ps_dec->ps_dec_err_status, 0, size);

    size = sizeof(sei);
    memset(ps_dec->ps_sei, 0, size);

    size = sizeof(dpb_commands_t);
    memset(ps_dec->ps_dpb_cmds, 0, size);

    size = sizeof(dec_bit_stream_t);
    memset(ps_dec->ps_bitstrm, 0, size);

    size = sizeof(dec_slice_params_t);
    memset(ps_dec->ps_cur_slice, 0, size);

    size = MAX(sizeof(dec_seq_params_t), sizeof(dec_pic_params_t));
    memset(ps_dec->pv_scratch_sps_pps, 0, size);

    size = sizeof(ctxt_inc_mb_info_t);
    memset(ps_dec->ps_left_mb_ctxt_info, 0, size);

    size = (sizeof(neighbouradd_t) << 2);
    memset(ps_dec->ps_left_mvpred_addr, 0 ,size);

    size = sizeof(buf_mgr_t) + ithread_get_mutex_lock_size();
    memset(ps_dec->pv_mv_buf_mgr, 0, size);

 /* Free any dynamic buffers that are allocated */
    ih264d_free_dynamic_bufs(ps_dec);

    ps_cur_slice = ps_dec->ps_cur_slice;
    ps_dec->init_done = 0;

    ps_dec->u4_num_cores = 1;

    ps_dec->u2_pic_ht = ps_dec->u2_pic_wd = 0;

    ps_dec->u1_separate_parse = DEFAULT_SEPARATE_PARSE;
    ps_dec->u4_app_disable_deblk_frm = 0;
    ps_dec->i4_degrade_type = 0;
    ps_dec->i4_degrade_pics = 0;

    ps_dec->i4_app_skip_mode = IVD_SKIP_NONE;
    ps_dec->i4_dec_skip_mode = IVD_SKIP_NONE;

    memset(ps_dec->ps_pps, 0,
 ((sizeof(dec_pic_params_t)) * MAX_NUM_PIC_PARAMS));
    memset(ps_dec->ps_sps, 0,
 ((sizeof(dec_seq_params_t)) * MAX_NUM_SEQ_PARAMS));

 /* Initialization of function pointers ih264d_deblock_picture function*/

    ps_dec->p_DeblockPicture[0] = ih264d_deblock_picture_non_mbaff;
    ps_dec->p_DeblockPicture[1] = ih264d_deblock_picture_mbaff;

    ps_dec->s_cab_dec_env.pv_codec_handle = ps_dec;

    ps_dec->u4_num_fld_in_frm = 0;

    ps_dec->ps_dpb_mgr->pv_codec_handle = ps_dec;

 /* Initialize the sei validity u4_flag with zero indiacting sei is not valid*/
    ps_dec->ps_sei->u1_is_valid = 0;

 /* decParams Initializations */
    ps_dec->ps_cur_pps = NULL;
    ps_dec->ps_cur_sps = NULL;
    ps_dec->u1_init_dec_flag = 0;
    ps_dec->u1_first_slice_in_stream = 1;
    ps_dec->u1_first_pb_nal_in_pic = 1;
    ps_dec->u1_last_pic_not_decoded = 0;
    ps_dec->u4_app_disp_width = 0;
    ps_dec->i4_header_decoded = 0;
    ps_dec->u4_total_frames_decoded = 0;

    ps_dec->i4_error_code = 0;
    ps_dec->i4_content_type = -1;
    ps_dec->ps_cur_slice->u1_mbaff_frame_flag = 0;

    ps_dec->ps_dec_err_status->u1_err_flag = ACCEPT_ALL_PICS; //REJECT_PB_PICS;
    ps_dec->ps_dec_err_status->u1_cur_pic_type = PIC_TYPE_UNKNOWN;
    ps_dec->ps_dec_err_status->u4_frm_sei_sync = SYNC_FRM_DEFAULT;
    ps_dec->ps_dec_err_status->u4_cur_frm = INIT_FRAME;
    ps_dec->ps_dec_err_status->u1_pic_aud_i = PIC_TYPE_UNKNOWN;

    ps_dec->u1_pr_sl_type = 0xFF;
    ps_dec->u2_mbx = 0xffff;
    ps_dec->u2_mby = 0;
    ps_dec->u2_total_mbs_coded = 0;

 /* POC initializations */
    ps_prev_poc = &ps_dec->s_prev_pic_poc;
    ps_cur_poc = &ps_dec->s_cur_pic_poc;
    ps_prev_poc->i4_pic_order_cnt_lsb = ps_cur_poc->i4_pic_order_cnt_lsb = 0;
    ps_prev_poc->i4_pic_order_cnt_msb = ps_cur_poc->i4_pic_order_cnt_msb = 0;
    ps_prev_poc->i4_delta_pic_order_cnt_bottom =
                    ps_cur_poc->i4_delta_pic_order_cnt_bottom = 0;
    ps_prev_poc->i4_delta_pic_order_cnt[0] =
                    ps_cur_poc->i4_delta_pic_order_cnt[0] = 0;
    ps_prev_poc->i4_delta_pic_order_cnt[1] =
                    ps_cur_poc->i4_delta_pic_order_cnt[1] = 0;
    ps_prev_poc->u1_mmco_equalto5 = ps_cur_poc->u1_mmco_equalto5 = 0;
    ps_prev_poc->i4_top_field_order_count = ps_cur_poc->i4_top_field_order_count =
 0;
    ps_prev_poc->i4_bottom_field_order_count =
                    ps_cur_poc->i4_bottom_field_order_count = 0;
    ps_prev_poc->u1_bot_field = ps_cur_poc->u1_bot_field = 0;
    ps_prev_poc->u1_mmco_equalto5 = ps_cur_poc->u1_mmco_equalto5 = 0;
    ps_prev_poc->i4_prev_frame_num_ofst = ps_cur_poc->i4_prev_frame_num_ofst = 0;
    ps_cur_slice->u1_mmco_equalto5 = 0;
    ps_cur_slice->u2_frame_num = 0;

    ps_dec->i4_max_poc = 0;
    ps_dec->i4_prev_max_display_seq = 0;
    ps_dec->u1_recon_mb_grp = 4;

 /* Field PIC initializations */
    ps_dec->u1_second_field = 0;
    ps_dec->s_prev_seq_params.u1_eoseq_pending = 0;

 /* Set the cropping parameters as zero */
    ps_dec->u2_crop_offset_y = 0;
    ps_dec->u2_crop_offset_uv = 0;

 /* The Initial Frame Rate Info is not Present */
    ps_dec->i4_vui_frame_rate = -1;
    ps_dec->i4_pic_type = -1;
    ps_dec->i4_frametype = -1;
    ps_dec->i4_content_type = -1;

    ps_dec->u1_res_changed = 0;


    ps_dec->u1_frame_decoded_flag = 0;

 /* Set the default frame seek mask mode */
    ps_dec->u4_skip_frm_mask = SKIP_NONE;

 /********************************************************/
 /* Initialize CAVLC residual decoding function pointers */
 /********************************************************/
    ps_dec->pf_cavlc_4x4res_block[0] = ih264d_cavlc_4x4res_block_totalcoeff_1;
    ps_dec->pf_cavlc_4x4res_block[1] =
                    ih264d_cavlc_4x4res_block_totalcoeff_2to10;
    ps_dec->pf_cavlc_4x4res_block[2] =
                    ih264d_cavlc_4x4res_block_totalcoeff_11to16;

    ps_dec->pf_cavlc_parse4x4coeff[0] = ih264d_cavlc_parse4x4coeff_n0to7;
    ps_dec->pf_cavlc_parse4x4coeff[1] = ih264d_cavlc_parse4x4coeff_n8;

    ps_dec->pf_cavlc_parse_8x8block[0] =
                    ih264d_cavlc_parse_8x8block_none_available;
    ps_dec->pf_cavlc_parse_8x8block[1] =
                    ih264d_cavlc_parse_8x8block_left_available;
    ps_dec->pf_cavlc_parse_8x8block[2] =
                    ih264d_cavlc_parse_8x8block_top_available;
    ps_dec->pf_cavlc_parse_8x8block[3] =
                    ih264d_cavlc_parse_8x8block_both_available;

 /***************************************************************************/
 /* Initialize Bs calculation function pointers for P and B, 16x16/non16x16 */
 /***************************************************************************/
    ps_dec->pf_fill_bs1[0][0] = ih264d_fill_bs1_16x16mb_pslice;
    ps_dec->pf_fill_bs1[0][1] = ih264d_fill_bs1_non16x16mb_pslice;

    ps_dec->pf_fill_bs1[1][0] = ih264d_fill_bs1_16x16mb_bslice;
    ps_dec->pf_fill_bs1[1][1] = ih264d_fill_bs1_non16x16mb_bslice;

    ps_dec->pf_fill_bs_xtra_left_edge[0] =
                    ih264d_fill_bs_xtra_left_edge_cur_frm;
    ps_dec->pf_fill_bs_xtra_left_edge[1] =
                    ih264d_fill_bs_xtra_left_edge_cur_fld;

 /* Initialize Reference Pic Buffers */
    ih264d_init_ref_bufs(ps_dec->ps_dpb_mgr);

    ps_dec->u2_prv_frame_num = 0;
    ps_dec->u1_top_bottom_decoded = 0;
    ps_dec->u1_dangling_field = 0;

    ps_dec->s_cab_dec_env.cabac_table = gau4_ih264d_cabac_table;

    ps_dec->pu1_left_mv_ctxt_inc = ps_dec->u1_left_mv_ctxt_inc_arr[0];
    ps_dec->pi1_left_ref_idx_ctxt_inc =
 &ps_dec->i1_left_ref_idx_ctx_inc_arr[0][0];
    ps_dec->pu1_left_yuv_dc_csbp = &ps_dec->u1_yuv_dc_csbp_topmb;

 /* ! */
 /* Initializing flush frame u4_flag */
    ps_dec->u1_flushfrm = 0;

 {
        ps_dec->s_cab_dec_env.pv_codec_handle = (void*)ps_dec;
        ps_dec->ps_bitstrm->pv_codec_handle = (void*)ps_dec;
        ps_dec->ps_cur_slice->pv_codec_handle = (void*)ps_dec;
        ps_dec->ps_dpb_mgr->pv_codec_handle = (void*)ps_dec;
 }

    memset(ps_dec->disp_bufs, 0, (MAX_DISP_BUFS_NEW) * sizeof(disp_buf_t));
    memset(ps_dec->u4_disp_buf_mapping, 0,

            (MAX_DISP_BUFS_NEW) * sizeof(UWORD32));
     memset(ps_dec->u4_disp_buf_to_be_freed, 0,
            (MAX_DISP_BUFS_NEW) * sizeof(UWORD32));
 
     ih264d_init_arch(ps_dec);
     ih264d_init_function_ptr(ps_dec);
    ps_dec->e_frm_out_mode = IVD_DISPLAY_FRAME_OUT;
    ps_dec->init_done = 1;

}
