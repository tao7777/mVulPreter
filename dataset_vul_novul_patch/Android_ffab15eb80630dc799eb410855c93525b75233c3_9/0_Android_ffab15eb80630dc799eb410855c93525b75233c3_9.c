void impeg2d_peek_next_start_code(dec_state_t *ps_dec)
{
 stream_t *ps_stream;
    ps_stream = &ps_dec->s_bit_stream;

     impeg2d_bit_stream_flush_to_byte_boundary(ps_stream);
 
     while ((impeg2d_bit_stream_nxt(ps_stream,START_CODE_PREFIX_LEN) != START_CODE_PREFIX)
        && (ps_dec->s_bit_stream.u4_offset < ps_dec->s_bit_stream.u4_max_offset))
     {
         impeg2d_bit_stream_get(ps_stream,8);
     }
 return;
}
