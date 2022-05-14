void impeg2d_dec_hdr(void *pv_dec,impeg2d_video_decode_ip_t *ps_ip,
 impeg2d_video_decode_op_t *ps_op)
{

    UWORD32 u4_bits_read;
 dec_state_t *ps_dec;

    ps_dec = (dec_state_t *)pv_dec;
    ps_op->s_ivd_video_decode_op_t.u4_error_code = 0;

    impeg2d_bit_stream_init(&(ps_dec->s_bit_stream),ps_ip->s_ivd_video_decode_ip_t.pv_stream_buffer,
        ps_ip->s_ivd_video_decode_ip_t.u4_num_Bytes);

 {
 {
            IMPEG2D_ERROR_CODES_T e_error;
            e_error = impeg2d_process_video_header(ps_dec);
 if ((IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE != e_error)
 {
                ps_op->s_ivd_video_decode_op_t.u4_error_code    = e_error;

                u4_bits_read     = impeg2d_bit_stream_num_bits_read(&ps_dec->s_bit_stream);

                ps_op->s_ivd_video_decode_op_t.u4_num_bytes_consumed = u4_bits_read>> 3;
 if(ps_op->s_ivd_video_decode_op_t.u4_num_bytes_consumed > ps_ip->s_ivd_video_decode_ip_t.u4_num_Bytes)
 {
                    ps_op->s_ivd_video_decode_op_t.u4_num_bytes_consumed = ps_ip->s_ivd_video_decode_ip_t.u4_num_Bytes;
 }

                 if(ps_op->s_ivd_video_decode_op_t.u4_error_code == 0)
                     ps_op->s_ivd_video_decode_op_t.u4_error_code = e_error;
 
 
                 impeg2d_next_code(ps_dec, SEQUENCE_HEADER_CODE);
                 return;
             }
 }
        ps_op->s_ivd_video_decode_op_t.u4_pic_ht = ps_dec->u2_vertical_size;
        ps_op->s_ivd_video_decode_op_t.u4_pic_wd = ps_dec->u2_horizontal_size;

        ps_op->s_ivd_video_decode_op_t.e_pic_type            = IV_NA_FRAME;
        ps_op->s_ivd_video_decode_op_t.u4_error_code        = IV_SUCCESS;

        u4_bits_read     = impeg2d_bit_stream_num_bits_read(&ps_dec->s_bit_stream);
        ps_op->s_ivd_video_decode_op_t.u4_num_bytes_consumed = u4_bits_read>> 3;
 if(ps_op->s_ivd_video_decode_op_t.u4_num_bytes_consumed > ps_ip->s_ivd_video_decode_ip_t.u4_num_Bytes)
 {
            ps_op->s_ivd_video_decode_op_t.u4_num_bytes_consumed = ps_ip->s_ivd_video_decode_ip_t.u4_num_Bytes;
 }
        ps_op->s_ivd_video_decode_op_t.u4_frame_decoded_flag = 0;
 /* MOD */
        ps_dec->u2_header_done = 1;

 }
}
