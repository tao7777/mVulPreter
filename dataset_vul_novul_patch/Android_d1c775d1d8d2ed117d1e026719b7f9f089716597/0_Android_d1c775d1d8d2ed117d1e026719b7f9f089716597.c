 INLINE void impeg2d_bit_stream_flush(void* pv_ctxt, UWORD32 u4_no_of_bits)
 {
     stream_t *ps_stream = (stream_t *)pv_ctxt;
    if ((ps_stream->u4_offset + 64) < ps_stream->u4_max_offset)
     {
         FLUSH_BITS(ps_stream->u4_offset,ps_stream->u4_buf,ps_stream->u4_buf_nxt,u4_no_of_bits,ps_stream->pu4_buf_aligned)
     }
    else
    {
        UWORD32     u4_temp;

        if (((ps_stream->u4_offset & 0x1f) + u4_no_of_bits) >= 32)
        {
            ps_stream->u4_buf              = ps_stream->u4_buf_nxt;
            ps_stream->u4_buf_nxt          = 0;
        }
        ps_stream->u4_offset += u4_no_of_bits;
    }
     return;
 }
