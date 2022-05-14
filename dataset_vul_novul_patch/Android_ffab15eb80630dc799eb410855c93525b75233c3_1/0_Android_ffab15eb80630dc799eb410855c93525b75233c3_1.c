IMPEG2D_ERROR_CODES_T impeg2d_dec_d_slice(dec_state_t *ps_dec)
{
    UWORD32 i;
 yuv_buf_t *ps_cur_frm_buf  = &ps_dec->s_cur_frm_buf;

 stream_t *ps_stream       = &ps_dec->s_bit_stream;
    UWORD8   *pu1_vld_buf;

    WORD16 i2_dc_diff;
    UWORD32 u4_frame_width = ps_dec->u2_frame_width;
    UWORD32 u4_frm_offset = 0;
 if(ps_dec->u2_picture_structure != FRAME_PICTURE)
 {
        u4_frame_width <<= 1;
 if(ps_dec->u2_picture_structure == BOTTOM_FIELD)
 {
            u4_frm_offset = ps_dec->u2_frame_width;
 }
 }

 do
 {

        UWORD32 u4_x_offset, u4_y_offset;
        UWORD32 u4_blk_pos;
        WORD16 i2_dc_val;

        UWORD32 u4_dst_x_offset     = u4_frm_offset + (ps_dec->u2_mb_x << 4);
        UWORD32 u4_dst_y_offset     = (ps_dec->u2_mb_y << 4) * u4_frame_width;
        UWORD8 *pu1_vld_buf8        = ps_cur_frm_buf->pu1_y + u4_dst_x_offset + u4_dst_y_offset;
        UWORD32 u4_dst_wd           = u4_frame_width;

         /*------------------------------------------------------------------*/
         /* Discard the Macroblock stuffing in case of MPEG-1 stream         */
         /*------------------------------------------------------------------*/
        while(impeg2d_bit_stream_nxt(ps_stream,MB_STUFFING_CODE_LEN) == MB_STUFFING_CODE &&
                ps_stream->u4_offset < ps_stream->u4_max_offset)
             impeg2d_bit_stream_flush(ps_stream,MB_STUFFING_CODE_LEN);
 
         /*------------------------------------------------------------------*/
 /* Flush 2 bits from bitstream [MB_Type and MacroBlockAddrIncrement]*/
 /*------------------------------------------------------------------*/
        impeg2d_bit_stream_flush(ps_stream,1);

 if(impeg2d_bit_stream_get(ps_stream, 1) != 0x01)
 {
 /* Ignore and continue decoding. */
 }

 /* Process LUMA blocks of the MB */
 for(i = 0; i < NUM_LUMA_BLKS; ++i)
 {

            u4_x_offset    = gai2_impeg2_blk_x_off[i];
            u4_y_offset    = gai2_impeg2_blk_y_off_frm[i] ;
            u4_blk_pos     = (u4_y_offset * u4_dst_wd) + u4_x_offset;
            pu1_vld_buf     = pu1_vld_buf8 + u4_blk_pos;

            i2_dc_diff = impeg2d_get_luma_dc_diff(ps_stream);
            i2_dc_val = ps_dec->u2_def_dc_pred[Y_LUMA] + i2_dc_diff;
            ps_dec->u2_def_dc_pred[Y_LUMA] = i2_dc_val;
            i2_dc_val = CLIP_U8(i2_dc_val);

            ps_dec->pf_memset_8bit_8x8_block(pu1_vld_buf, i2_dc_val, u4_dst_wd);
 }



 /* Process U block of the MB */

        u4_dst_x_offset                >>= 1;
        u4_dst_y_offset                >>= 2;
        u4_dst_wd                      >>= 1;
        pu1_vld_buf                     = ps_cur_frm_buf->pu1_u + u4_dst_x_offset + u4_dst_y_offset;
        i2_dc_diff                     = impeg2d_get_chroma_dc_diff(ps_stream);
        i2_dc_val                      = ps_dec->u2_def_dc_pred[U_CHROMA] + i2_dc_diff;
        ps_dec->u2_def_dc_pred[U_CHROMA] = i2_dc_val;
        i2_dc_val = CLIP_U8(i2_dc_val);
        ps_dec->pf_memset_8bit_8x8_block(pu1_vld_buf, i2_dc_val, u4_dst_wd);


 /* Process V block of the MB */

        pu1_vld_buf                     = ps_cur_frm_buf->pu1_v + u4_dst_x_offset + u4_dst_y_offset;
        i2_dc_diff                     = impeg2d_get_chroma_dc_diff(ps_stream);
        i2_dc_val                      = ps_dec->u2_def_dc_pred[V_CHROMA] + i2_dc_diff;
        ps_dec->u2_def_dc_pred[V_CHROMA] = i2_dc_val;
        i2_dc_val = CLIP_U8(i2_dc_val);
        ps_dec->pf_memset_8bit_8x8_block(pu1_vld_buf, i2_dc_val, u4_dst_wd);

 /* Common MB processing Steps */


        ps_dec->u2_num_mbs_left--;
        ps_dec->u2_mb_x++;

 if(ps_dec->s_bit_stream.u4_offset > ps_dec->s_bit_stream.u4_max_offset)
 {
 return IMPEG2D_BITSTREAM_BUFF_EXCEEDED_ERR;
 }
 else if (ps_dec->u2_mb_x == ps_dec->u2_num_horiz_mb)
 {
            ps_dec->u2_mb_x = 0;
            ps_dec->u2_mb_y++;

 }

 /* Flush end of macro block */
        impeg2d_bit_stream_flush(ps_stream,1);
 }
 while(ps_dec->u2_num_mbs_left != 0 && impeg2d_bit_stream_nxt(&ps_dec->s_bit_stream,23) != 0x0);
 return (IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE;
}/* End of impeg2d_dec_d_slice() */
