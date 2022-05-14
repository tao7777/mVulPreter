WORD32 ih264d_decode_gaps_in_frame_num(dec_struct_t *ps_dec,
                                       UWORD16 u2_frame_num)
{
    UWORD32 u4_next_frm_num, u4_start_frm_num;
    UWORD32 u4_max_frm_num;
 pocstruct_t s_tmp_poc;
    WORD32 i4_poc;
 dec_slice_params_t *ps_cur_slice;

 dec_pic_params_t *ps_pic_params;
    WORD8 i1_gap_idx;
    WORD32 *i4_gaps_start_frm_num;
 dpb_manager_t *ps_dpb_mgr;
    WORD32 i4_frame_gaps;
    WORD8 *pi1_gaps_per_seq;
    WORD32 ret;

    ps_cur_slice = ps_dec->ps_cur_slice;
 if(ps_cur_slice->u1_field_pic_flag)
 {
 if(ps_dec->u2_prev_ref_frame_num == u2_frame_num)
 return 0;
 }

    u4_next_frm_num = ps_dec->u2_prev_ref_frame_num + 1;
    u4_max_frm_num = ps_dec->ps_cur_sps->u2_u4_max_pic_num_minus1 + 1;

 if(u4_next_frm_num >= u4_max_frm_num)
 {
        u4_next_frm_num -= u4_max_frm_num;
 }

 if(u4_next_frm_num == u2_frame_num)
 {
 return (0);
 }

 if((ps_dec->u1_nal_unit_type == IDR_SLICE_NAL)
 && (u4_next_frm_num >= u2_frame_num))
 {
 return (0);
 }
    u4_start_frm_num = u4_next_frm_num;

    s_tmp_poc.i4_pic_order_cnt_lsb = 0;
    s_tmp_poc.i4_delta_pic_order_cnt_bottom = 0;
    s_tmp_poc.i4_pic_order_cnt_lsb = 0;
    s_tmp_poc.i4_delta_pic_order_cnt_bottom = 0;
    s_tmp_poc.i4_delta_pic_order_cnt[0] = 0;
    s_tmp_poc.i4_delta_pic_order_cnt[1] = 0;

 
     ps_cur_slice = ps_dec->ps_cur_slice;
     ps_pic_params = ps_dec->ps_cur_pps;
 
     i4_frame_gaps = 0;
     ps_dpb_mgr = ps_dec->ps_dpb_mgr;

 /* Find a empty slot to store gap seqn info */
    i4_gaps_start_frm_num = ps_dpb_mgr->ai4_gaps_start_frm_num;
 for(i1_gap_idx = 0; i1_gap_idx < MAX_FRAMES; i1_gap_idx++)
 {
 if(INVALID_FRAME_NUM == i4_gaps_start_frm_num[i1_gap_idx])
 break;
 }
 if(MAX_FRAMES == i1_gap_idx)
 {
        UWORD32 i4_error_code;
        i4_error_code = ERROR_DBP_MANAGER_T;
 return i4_error_code;
 }

    i4_poc = 0;
    i4_gaps_start_frm_num[i1_gap_idx] = u4_start_frm_num;
    ps_dpb_mgr->ai4_gaps_end_frm_num[i1_gap_idx] = u2_frame_num - 1;
    pi1_gaps_per_seq = ps_dpb_mgr->ai1_gaps_per_seq;
    pi1_gaps_per_seq[i1_gap_idx] = 0;
 while(u4_next_frm_num != u2_frame_num)
 {
        ih264d_delete_nonref_nondisplay_pics(ps_dpb_mgr);
 if(ps_pic_params->ps_sps->u1_pic_order_cnt_type)
 {
 /* allocate a picture buffer and insert it as ST node */
            ret = ih264d_decode_pic_order_cnt(0, u4_next_frm_num,
 &ps_dec->s_prev_pic_poc,
 &s_tmp_poc, ps_cur_slice,
                                              ps_pic_params, 1, 0, 0,
 &i4_poc);
 if(ret != OK)
 return ret;

 /* Display seq no calculations */
 if(i4_poc >= ps_dec->i4_max_poc)
                ps_dec->i4_max_poc = i4_poc;
 /* IDR Picture or POC wrap around */
 if(i4_poc == 0)
 {
                ps_dec->i4_prev_max_display_seq =
                                ps_dec->i4_prev_max_display_seq
 + ps_dec->i4_max_poc
 + ps_dec->u1_max_dec_frame_buffering
 + 1;
                ps_dec->i4_max_poc = 0;
 }

            ps_cur_slice->u1_mmco_equalto5 = 0;
            ps_cur_slice->u2_frame_num = u4_next_frm_num;
 }

 if(ps_dpb_mgr->i1_poc_buf_id_entries
 >= ps_dec->u1_max_dec_frame_buffering)
 {
            ret = ih264d_assign_display_seq(ps_dec);
 if(ret != OK)
 return ret;
 }

        ret = ih264d_insert_pic_in_display_list(
                        ps_dec->ps_dpb_mgr, (WORD8) DO_NOT_DISP,
 (WORD32)(ps_dec->i4_prev_max_display_seq + i4_poc),
                        u4_next_frm_num);
 if(ret != OK)
 return ret;

        pi1_gaps_per_seq[i1_gap_idx]++;
        ret = ih264d_do_mmco_for_gaps(ps_dpb_mgr,
                                ps_dec->ps_cur_sps->u1_num_ref_frames);
 if(ret != OK)
 return ret;

        ih264d_delete_nonref_nondisplay_pics(ps_dpb_mgr);

        u4_next_frm_num++;
 if(u4_next_frm_num >= u4_max_frm_num)
 {
            u4_next_frm_num -= u4_max_frm_num;
 }

        i4_frame_gaps++;
 }

 return OK;
}
