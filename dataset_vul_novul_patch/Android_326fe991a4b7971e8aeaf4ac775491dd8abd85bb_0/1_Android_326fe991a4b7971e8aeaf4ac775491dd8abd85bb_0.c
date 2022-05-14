WORD32 ih264d_end_of_pic(dec_struct_t *ps_dec,
                       UWORD8 u1_is_idr_slice,
                       UWORD16 u2_frame_num)
{

     dec_slice_params_t *ps_cur_slice = ps_dec->ps_cur_slice;
     WORD32 ret;
 
    ps_dec->u1_first_pb_nal_in_pic = 1;
     ps_dec->u2_mbx = 0xffff;
     ps_dec->u2_mby = 0;
     {
 dec_err_status_t * ps_err = ps_dec->ps_dec_err_status;
 if(ps_err->u1_err_flag & REJECT_CUR_PIC)
 {
            ih264d_err_pic_dispbuf_mgr(ps_dec);
 return ERROR_NEW_FRAME_EXPECTED;
 }
 }

    H264_MUTEX_LOCK(&ps_dec->process_disp_mutex);
    ret = ih264d_end_of_pic_processing(ps_dec);
 if(ret != OK)
 return ret;
    ps_dec->u2_total_mbs_coded = 0;
 /*--------------------------------------------------------------------*/
 /* ih264d_decode_pic_order_cnt - calculate the Pic Order Cnt                    */
 /* Needed to detect end of picture                                    */
 /*--------------------------------------------------------------------*/
 {
 pocstruct_t *ps_prev_poc = &ps_dec->s_prev_pic_poc;
 pocstruct_t *ps_cur_poc = &ps_dec->s_cur_pic_poc;
 if((0 == u1_is_idr_slice) && ps_cur_slice->u1_nal_ref_idc)
            ps_dec->u2_prev_ref_frame_num = ps_cur_slice->u2_frame_num;

 if(u1_is_idr_slice || ps_cur_slice->u1_mmco_equalto5)
            ps_dec->u2_prev_ref_frame_num = 0;

 if(ps_dec->ps_cur_sps->u1_gaps_in_frame_num_value_allowed_flag)
 {
            ret = ih264d_decode_gaps_in_frame_num(ps_dec, u2_frame_num);
 if(ret != OK)
 return ret;
 }

        ps_prev_poc->i4_prev_frame_num_ofst = ps_cur_poc->i4_prev_frame_num_ofst;
        ps_prev_poc->u2_frame_num = ps_cur_poc->u2_frame_num;
        ps_prev_poc->u1_mmco_equalto5 = ps_cur_slice->u1_mmco_equalto5;
 if(ps_cur_slice->u1_nal_ref_idc)
 {
            ps_prev_poc->i4_pic_order_cnt_lsb = ps_cur_poc->i4_pic_order_cnt_lsb;
            ps_prev_poc->i4_pic_order_cnt_msb = ps_cur_poc->i4_pic_order_cnt_msb;
            ps_prev_poc->i4_delta_pic_order_cnt_bottom =
                            ps_cur_poc->i4_delta_pic_order_cnt_bottom;
            ps_prev_poc->i4_delta_pic_order_cnt[0] =
                            ps_cur_poc->i4_delta_pic_order_cnt[0];
            ps_prev_poc->i4_delta_pic_order_cnt[1] =
                            ps_cur_poc->i4_delta_pic_order_cnt[1];
            ps_prev_poc->u1_bot_field = ps_cur_poc->u1_bot_field;
 }
 }

    H264_MUTEX_UNLOCK(&ps_dec->process_disp_mutex);

 return OK;
}
