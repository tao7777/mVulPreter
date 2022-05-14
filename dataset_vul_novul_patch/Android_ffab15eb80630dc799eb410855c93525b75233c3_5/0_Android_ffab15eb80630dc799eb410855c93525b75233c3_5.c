IMPEG2D_ERROR_CODES_T impeg2d_dec_slice(dec_state_t *ps_dec)
{
 stream_t *ps_stream;
    UWORD32 u4_slice_vertical_position;
    UWORD32 u4_slice_vertical_position_extension;
    IMPEG2D_ERROR_CODES_T e_error;

    ps_stream = &ps_dec->s_bit_stream;

 /*------------------------------------------------------------------------*/
 /* All the profiles supported require restricted slice structure. Hence   */
 /* there is no need to store slice_vertical_position. Note that max       */
 /* height supported does not exceed 2800 and scalablity is not supported  */
 /*------------------------------------------------------------------------*/

 /* Remove the slice start code */
    impeg2d_bit_stream_flush(ps_stream,START_CODE_PREFIX_LEN);
    u4_slice_vertical_position = impeg2d_bit_stream_get(ps_stream, 8);
 if(u4_slice_vertical_position > 2800)
 {
        u4_slice_vertical_position_extension = impeg2d_bit_stream_get(ps_stream, 3);
        u4_slice_vertical_position += (u4_slice_vertical_position_extension << 7);
 }

 if((u4_slice_vertical_position > ps_dec->u2_num_vert_mb) ||
 (u4_slice_vertical_position == 0))
 {
 return IMPEG2D_INVALID_VERT_SIZE;
 }

    u4_slice_vertical_position--;
 if (ps_dec->u2_mb_y != u4_slice_vertical_position)
 {
        ps_dec->u2_mb_y    = u4_slice_vertical_position;
        ps_dec->u2_mb_x    = 0;
 }
    ps_dec->u2_first_mb = 1;

 /*------------------------------------------------------------------------*/
 /* Quant scale code decoding                                              */
 /*------------------------------------------------------------------------*/
 {
        UWORD16 u2_quant_scale_code;
        u2_quant_scale_code = impeg2d_bit_stream_get(ps_stream,5);
        ps_dec->u1_quant_scale = (ps_dec->u2_q_scale_type) ?
            gau1_impeg2_non_linear_quant_scale[u2_quant_scale_code] : (u2_quant_scale_code << 1);
 }

 if (impeg2d_bit_stream_nxt(ps_stream,1) == 1)

     {
         impeg2d_bit_stream_flush(ps_stream,9);
         /* Flush extra bit information */
        while (impeg2d_bit_stream_nxt(ps_stream,1) == 1 &&
               ps_stream->u4_offset < ps_stream->u4_max_offset)
         {
             impeg2d_bit_stream_flush(ps_stream,9);
         }
 }
    impeg2d_bit_stream_get_bit(ps_stream);

 /* Reset the DC predictors to reset values given in Table 7.2 at the start*/
 /* of slice data */
    ps_dec->u2_def_dc_pred[Y_LUMA] = 128 << ps_dec->u2_intra_dc_precision;
    ps_dec->u2_def_dc_pred[U_CHROMA] = 128 << ps_dec->u2_intra_dc_precision;
    ps_dec->u2_def_dc_pred[V_CHROMA] = 128 << ps_dec->u2_intra_dc_precision;
 /*------------------------------------------------------------------------*/
 /* dec->DecMBsinSlice() implements the following psuedo code from standard*/
 /* do                                                                     */
 /* {                                                                      */
 /*      macroblock()                                                      */
 /* } while (impeg2d_bit_stream_nxt() != '000 0000 0000 0000 0000 0000')      */
 /*------------------------------------------------------------------------*/

    e_error = ps_dec->pf_decode_slice(ps_dec);
 if ((IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE != e_error)
 {
 return e_error;
 }

 /* Check for the MBy index instead of number of MBs left, because the
     * number of MBs left in case of multi-thread decode is the number of MBs
     * in that row only
     */
 if(ps_dec->u2_mb_y < ps_dec->u2_num_vert_mb)
        impeg2d_next_start_code(ps_dec);

 return (IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE;
}
