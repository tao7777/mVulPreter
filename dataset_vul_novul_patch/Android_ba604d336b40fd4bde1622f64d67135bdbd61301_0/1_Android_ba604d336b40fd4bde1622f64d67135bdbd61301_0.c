WORD32 impeg2d_get_slice_pos(dec_state_multi_core_t *ps_dec_state_multi_core)
{
    WORD32 u4_bits;
    WORD32 i4_row;


 dec_state_t *ps_dec = ps_dec_state_multi_core->ps_dec_state[0];
    WORD32 i4_prev_row;
 stream_t s_bitstrm;
    WORD32 i4_start_row;
    WORD32 i4_slice_bistream_ofst;
    WORD32 i;
    s_bitstrm = ps_dec->s_bit_stream;
    i4_prev_row = -1;

    ps_dec_state_multi_core->ps_dec_state[0]->i4_start_mb_y = 0;
    ps_dec_state_multi_core->ps_dec_state[1]->i4_start_mb_y = -1;
    ps_dec_state_multi_core->ps_dec_state[2]->i4_start_mb_y = -1;
    ps_dec_state_multi_core->ps_dec_state[3]->i4_start_mb_y = -1;

    ps_dec_state_multi_core->ps_dec_state[0]->i4_end_mb_y = ps_dec->u2_num_vert_mb;
    ps_dec_state_multi_core->ps_dec_state[1]->i4_end_mb_y = -1;
    ps_dec_state_multi_core->ps_dec_state[2]->i4_end_mb_y = -1;
    ps_dec_state_multi_core->ps_dec_state[3]->i4_end_mb_y = -1;

 if(ps_dec->i4_num_cores == 1)
 return 0;
 /* Reset the jobq to start of the jobq buffer */
    impeg2_jobq_reset((jobq_t *)ps_dec->pv_jobq);

    i4_start_row = -1;
    i4_slice_bistream_ofst = 0;
 while(1)
 {
        WORD32 i4_is_slice;

 if(s_bitstrm.u4_offset + START_CODE_LEN >= s_bitstrm.u4_max_offset)
 {
 break;
 }
        u4_bits = impeg2d_bit_stream_nxt(&s_bitstrm,START_CODE_LEN);

        i4_row = u4_bits & 0xFF;

 /* Detect end of frame */
        i4_is_slice = (((u4_bits >> 8) == 0x01) && (i4_row) && (i4_row <= ps_dec->u2_num_vert_mb));
 if(!i4_is_slice)
 break;


         i4_row -= 1;
 
 
        if(i4_prev_row != i4_row)
         {
             /* Create a job for previous slice row */
             if(i4_start_row != -1)
 {
 job_t s_job;
                IV_API_CALL_STATUS_T ret;
                s_job.i2_start_mb_y = i4_start_row;
                s_job.i2_end_mb_y = i4_row;
                s_job.i4_cmd = CMD_PROCESS;
                s_job.i4_bistream_ofst = i4_slice_bistream_ofst;
                ret = impeg2_jobq_queue(ps_dec->pv_jobq, &s_job, sizeof(s_job), 1, 0);
 if(ret != IV_SUCCESS)
 return ret;

 }
 /* Store current slice's bitstream offset */
            i4_slice_bistream_ofst = s_bitstrm.u4_offset >> 3;
            i4_slice_bistream_ofst -= (size_t)s_bitstrm.pv_bs_buf & 3;
            i4_prev_row = i4_row;


             /* Store current slice's row position */
             i4_start_row = i4_row;
 
         }
 
 
        impeg2d_bit_stream_flush(&s_bitstrm, START_CODE_LEN);

 /* Flush the bytes till a  start code is encountered  */
 while(impeg2d_bit_stream_nxt(&s_bitstrm, 24) != START_CODE_PREFIX)
 {
            impeg2d_bit_stream_get(&s_bitstrm, 8);

 if(s_bitstrm.u4_offset >= s_bitstrm.u4_max_offset)
 {
 break;
 }
 }
 }

 /* Create job for the last slice row */
 {
 job_t s_job;
        IV_API_CALL_STATUS_T e_ret;
        s_job.i2_start_mb_y = i4_start_row;
        s_job.i2_end_mb_y = ps_dec->u2_num_vert_mb;
        s_job.i4_cmd = CMD_PROCESS;
        s_job.i4_bistream_ofst = i4_slice_bistream_ofst;
        e_ret = impeg2_jobq_queue(ps_dec->pv_jobq, &s_job, sizeof(s_job), 1, 0);
 if(e_ret != IV_SUCCESS)
 return e_ret;

 }
 if((NULL != ps_dec->ps_disp_pic) && ((0 == ps_dec->u4_share_disp_buf) || (IV_YUV_420P != ps_dec->i4_chromaFormat)))
 {
 for(i = 0; i < ps_dec->u2_vertical_size; i+=64)
 {
 job_t s_job;
            IV_API_CALL_STATUS_T ret;
            s_job.i2_start_mb_y = i;
            s_job.i2_start_mb_y >>= 4;
            s_job.i2_end_mb_y = (i + 64);
            s_job.i2_end_mb_y >>= 4;
            s_job.i4_cmd = CMD_FMTCONV;
            s_job.i4_bistream_ofst = 0;
            ret = impeg2_jobq_queue(ps_dec->pv_jobq, &s_job, sizeof(s_job), 1, 0);
 if(ret != IV_SUCCESS)
 return ret;

 }
 }

    impeg2_jobq_terminate(ps_dec->pv_jobq);
    ps_dec->i4_bytes_consumed = s_bitstrm.u4_offset >> 3;
    ps_dec->i4_bytes_consumed -= ((size_t)s_bitstrm.pv_bs_buf & 3);

 return 0;
}
