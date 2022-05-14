void impeg2d_dec_pic_data_thread(dec_state_t *ps_dec)
{
    WORD32 i4_continue_decode;

    WORD32 i4_cur_row, temp;
    UWORD32 u4_bits_read;
    WORD32 i4_dequeue_job;
    IMPEG2D_ERROR_CODES_T e_error;

    i4_cur_row = ps_dec->u2_mb_y + 1;

    i4_continue_decode = 1;

    i4_dequeue_job = 1;
 do
 {
 if(i4_cur_row > ps_dec->u2_num_vert_mb)
 {
            i4_continue_decode = 0;
 break;
 }

 {
 if((ps_dec->i4_num_cores> 1) && (i4_dequeue_job))
 {
 job_t s_job;
                IV_API_CALL_STATUS_T e_ret;
                UWORD8 *pu1_buf;

                e_ret = impeg2_jobq_dequeue(ps_dec->pv_jobq, &s_job, sizeof(s_job), 1, 1);
 if(e_ret != IV_SUCCESS)
 break;

 if(CMD_PROCESS == s_job.i4_cmd)
 {
                    pu1_buf = ps_dec->pu1_inp_bits_buf + s_job.i4_bistream_ofst;
                    impeg2d_bit_stream_init(&(ps_dec->s_bit_stream), pu1_buf,
 (ps_dec->u4_num_inp_bytes - s_job.i4_bistream_ofst) + 8);
                    i4_cur_row      = s_job.i2_start_mb_y;
                    ps_dec->i4_start_mb_y = s_job.i2_start_mb_y;
                    ps_dec->i4_end_mb_y = s_job.i2_end_mb_y;
                    ps_dec->u2_mb_x = 0;
                    ps_dec->u2_mb_y = ps_dec->i4_start_mb_y;
                    ps_dec->u2_num_mbs_left = (ps_dec->i4_end_mb_y - ps_dec->i4_start_mb_y) * ps_dec->u2_num_horiz_mb;

 }
 else
 {
                    WORD32 start_row;
                    WORD32 num_rows;
                    start_row = s_job.i2_start_mb_y << 4;
                    num_rows = MIN((s_job.i2_end_mb_y << 4), ps_dec->u2_vertical_size);
                    num_rows -= start_row;
                    impeg2d_format_convert(ps_dec, ps_dec->ps_disp_pic,
                                        ps_dec->ps_disp_frm_buf,
                                        start_row, num_rows);
 break;

 }

 }
            e_error = impeg2d_dec_slice(ps_dec);

 if ((IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE != e_error)
 {
                impeg2d_next_start_code(ps_dec);
 }
 }

 /* Detecting next slice start code */
 while(1)
 {
            u4_bits_read = impeg2d_bit_stream_nxt(&ps_dec->s_bit_stream,START_CODE_LEN);

             temp = u4_bits_read & 0xFF;
             i4_continue_decode = (((u4_bits_read >> 8) == 0x01) && (temp) && (temp <= 0xAF));
 
             if(i4_continue_decode)
             {
                 /* If the slice is from the same row, then continue decoding without dequeue */
 if((temp - 1) == i4_cur_row)
 {
                    i4_dequeue_job = 0;
 break;
 }

 if(temp < ps_dec->i4_end_mb_y)
 {
                    i4_cur_row = ps_dec->u2_mb_y;
 }
 else
 {
                    i4_dequeue_job = 1;
 }
 break;

 }
 else
 break;
 }

 }while(i4_continue_decode);
 if(ps_dec->i4_num_cores > 1)
 {
 while(1)
 {
 job_t s_job;
            IV_API_CALL_STATUS_T e_ret;

            e_ret = impeg2_jobq_dequeue(ps_dec->pv_jobq, &s_job, sizeof(s_job), 1, 1);
 if(e_ret != IV_SUCCESS)
 break;
 if(CMD_FMTCONV == s_job.i4_cmd)
 {
                WORD32 start_row;
                WORD32 num_rows;
                start_row = s_job.i2_start_mb_y << 4;
                num_rows = MIN((s_job.i2_end_mb_y << 4), ps_dec->u2_vertical_size);
                num_rows -= start_row;
                impeg2d_format_convert(ps_dec, ps_dec->ps_disp_pic,
                                    ps_dec->ps_disp_frm_buf,
                                    start_row, num_rows);
 }
 }
 }
 else
 {
 if((NULL != ps_dec->ps_disp_pic) && ((0 == ps_dec->u4_share_disp_buf) || (IV_YUV_420P != ps_dec->i4_chromaFormat)))
            impeg2d_format_convert(ps_dec, ps_dec->ps_disp_pic,
                            ps_dec->ps_disp_frm_buf,
 0, ps_dec->u2_vertical_size);
 }
}
