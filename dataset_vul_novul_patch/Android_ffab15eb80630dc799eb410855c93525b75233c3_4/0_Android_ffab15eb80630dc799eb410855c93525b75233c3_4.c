IMPEG2D_ERROR_CODES_T impeg2d_dec_seq_ext_data(dec_state_t *ps_dec)
{
 stream_t *ps_stream;
    UWORD32     u4_start_code;
    IMPEG2D_ERROR_CODES_T e_error;

    e_error = (IMPEG2D_ERROR_CODES_T) IVD_ERROR_NONE;

    ps_stream      = &ps_dec->s_bit_stream;

     u4_start_code = impeg2d_bit_stream_nxt(ps_stream,START_CODE_LEN);
     while( (u4_start_code == EXTENSION_START_CODE ||
             u4_start_code == USER_DATA_START_CODE) &&
            (IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE == e_error &&
            (ps_stream->u4_offset < ps_stream->u4_max_offset))
     {
         if(u4_start_code == USER_DATA_START_CODE)
         {
            impeg2d_dec_user_data(ps_dec);
 }
 else
 {
            impeg2d_bit_stream_flush(ps_stream,START_CODE_LEN);
            u4_start_code   = impeg2d_bit_stream_nxt(ps_stream,EXT_ID_LEN);
 switch(u4_start_code)
 {
 case SEQ_DISPLAY_EXT_ID:
                impeg2d_dec_seq_disp_ext(ps_dec);
 break;
 case SEQ_SCALABLE_EXT_ID:
                e_error = IMPEG2D_SCALABILITIY_NOT_SUPPORTED;
 break;
 default:
 /* In case its a reserved extension code */
                impeg2d_bit_stream_flush(ps_stream,EXT_ID_LEN);
                impeg2d_peek_next_start_code(ps_dec);
 break;
 }
 }
        u4_start_code = impeg2d_bit_stream_nxt(ps_stream,START_CODE_LEN);
 }
 return e_error;
}
