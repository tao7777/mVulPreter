WORD32 ih264d_end_of_pic(dec_struct_t *ps_dec,
WORD32 ih264d_end_of_pic(dec_struct_t *ps_dec)
 {
     dec_slice_params_t *ps_cur_slice = ps_dec->ps_cur_slice;
     WORD32 ret;
 
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
     /*--------------------------------------------------------------------*/
     /* ih264d_decode_pic_order_cnt - calculate the Pic Order Cnt                    */
     /* Needed to detect end of picture                                    */
     /*--------------------------------------------------------------------*/
 
     H264_MUTEX_UNLOCK(&ps_dec->process_disp_mutex);
 
 return OK;
}
