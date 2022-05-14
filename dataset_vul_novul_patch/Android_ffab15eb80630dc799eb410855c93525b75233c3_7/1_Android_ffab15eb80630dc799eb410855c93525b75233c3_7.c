void impeg2d_flush_ext_and_user_data(dec_state_t *ps_dec)
{
    UWORD32 u4_start_code;
 stream_t *ps_stream;


     ps_stream    = &ps_dec->s_bit_stream;
     u4_start_code = impeg2d_bit_stream_nxt(ps_stream,START_CODE_LEN);
 
    while(u4_start_code == EXTENSION_START_CODE || u4_start_code == USER_DATA_START_CODE)
     {
         impeg2d_bit_stream_flush(ps_stream,START_CODE_LEN);
        while(impeg2d_bit_stream_nxt(ps_stream,START_CODE_PREFIX_LEN) != START_CODE_PREFIX)
         {
             impeg2d_bit_stream_flush(ps_stream,8);
         }
        u4_start_code = impeg2d_bit_stream_nxt(ps_stream,START_CODE_LEN);
 }
}
