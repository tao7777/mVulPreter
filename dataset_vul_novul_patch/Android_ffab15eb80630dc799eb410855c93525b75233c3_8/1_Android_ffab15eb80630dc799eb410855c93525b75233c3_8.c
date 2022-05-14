void impeg2d_next_code(dec_state_t *ps_dec, UWORD32 u4_start_code_val)
{
 stream_t *ps_stream;

     ps_stream = &ps_dec->s_bit_stream;
     impeg2d_bit_stream_flush_to_byte_boundary(ps_stream);
 
    while ((impeg2d_bit_stream_nxt(ps_stream,START_CODE_LEN) != u4_start_code_val)
        && (ps_dec->s_bit_stream.u4_offset <= ps_dec->s_bit_stream.u4_max_offset))
     {
 
         if (impeg2d_bit_stream_get(ps_stream,8) != 0)
 {
 /* Ignore stuffing bit errors. */
 }

 }
 return;
}
