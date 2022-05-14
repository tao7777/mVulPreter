 UWORD16 impeg2d_get_mb_addr_incr(stream_t *ps_stream)
 {
     UWORD16 u2_mb_addr_incr = 0;
    while (impeg2d_bit_stream_nxt(ps_stream,MB_ESCAPE_CODE_LEN) == MB_ESCAPE_CODE)
     {
         impeg2d_bit_stream_flush(ps_stream,MB_ESCAPE_CODE_LEN);
         u2_mb_addr_incr += 33;
 }
    u2_mb_addr_incr += impeg2d_dec_vld_symbol(ps_stream,gai2_impeg2d_mb_addr_incr,MB_ADDR_INCR_LEN) +
        MB_ADDR_INCR_OFFSET;
 return(u2_mb_addr_incr);
}
