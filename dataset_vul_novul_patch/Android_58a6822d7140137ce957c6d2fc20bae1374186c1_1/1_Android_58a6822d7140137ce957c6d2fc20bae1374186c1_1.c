void impeg2d_dec_pnb_mb_params(dec_state_t *ps_dec)
 {
     stream_t *ps_stream = &ps_dec->s_bit_stream;
     UWORD16 u2_mb_addr_incr;
    UWORD16 u2_total_len;
    UWORD16 u2_len;
    UWORD16 u2_mb_type;
    UWORD32 u4_next_word;
 const dec_mb_params_t *ps_dec_mb_params;
 if(impeg2d_bit_stream_nxt(ps_stream,1) == 1)
 {
        impeg2d_bit_stream_flush(ps_stream,1);

 }
 else
 {
        u2_mb_addr_incr = impeg2d_get_mb_addr_incr(ps_stream);

 if(ps_dec->u2_first_mb)
 {
 /****************************************************************/
 /* Section 6.3.17                                               */
 /* The first MB of a slice cannot be skipped                    */
 /* But the mb_addr_incr can be > 1, because at the beginning of */
 /* a slice, it indicates the offset from the last MB in the     */
 /* previous row. Hence for the first slice in a row, the        */
 /* mb_addr_incr needs to be 1.                                  */
 /****************************************************************/
 /* MB_x is set to zero whenever MB_y changes.                   */
            ps_dec->u2_mb_x = u2_mb_addr_incr - 1;
 /* For error resilience */
            ps_dec->u2_mb_x = MIN(ps_dec->u2_mb_x, (ps_dec->u2_num_horiz_mb - 1));

 /****************************************************************/
 /* mb_addr_incr is forced to 1 because in this decoder it is used */
 /* more as an indicator of the number of MBs skipped than the   */
 /* as defined by the standard (Section 6.3.17)                  */
 /****************************************************************/
            u2_mb_addr_incr = 1;
            ps_dec->u2_first_mb = 0;
 }
 else
 {
 /****************************************************************/
 /* In MPEG-2, the last MB of the row cannot be skipped and the  */
 /* mb_addr_incr cannot be such that it will take the current MB   */
 /* beyond the current row                                       */
 /* In MPEG-1, the slice could start and end anywhere and is not */
 /* restricted to a row like in MPEG-2. Hence this check should  */
 /* not be done for MPEG-1 streams.                              */
 /****************************************************************/
 if(ps_dec->u2_is_mpeg2 &&
 ((ps_dec->u2_mb_x + u2_mb_addr_incr) > ps_dec->u2_num_horiz_mb))
 {
                u2_mb_addr_incr    = ps_dec->u2_num_horiz_mb - ps_dec->u2_mb_x;
 }


            impeg2d_dec_skip_mbs(ps_dec, (UWORD16)(u2_mb_addr_incr - 1));
 }

 }
    u4_next_word = (UWORD16)impeg2d_bit_stream_nxt(ps_stream,16);
 /*-----------------------------------------------------------------------*/
 /* MB type                                                               */
 /*-----------------------------------------------------------------------*/
 {
        u2_mb_type   = ps_dec->pu2_mb_type[BITS((UWORD16)u4_next_word,15,10)];
        u2_len      = BITS(u2_mb_type,15,8);
        u2_total_len = u2_len;
        u4_next_word = (UWORD16)LSW((UWORD16)u4_next_word << u2_len);
 }
 /*-----------------------------------------------------------------------*/
 /* motion type                                                           */
 /*-----------------------------------------------------------------------*/
 {
        WORD32 i4_motion_type = ps_dec->u2_motion_type;

 if((u2_mb_type & MB_FORW_OR_BACK) &&  ps_dec->u2_read_motion_type)
 {
            ps_dec->u2_motion_type = BITS((UWORD16)u4_next_word,15,14);
            u2_total_len += MB_MOTION_TYPE_LEN;
            u4_next_word = (UWORD16)LSW((UWORD16)u4_next_word << MB_MOTION_TYPE_LEN);
            i4_motion_type     = ps_dec->u2_motion_type;

 }


 if ((u2_mb_type & MB_FORW_OR_BACK) &&
 ((i4_motion_type == 0) ||
 (i4_motion_type == 3) ||
 (i4_motion_type == 4) ||
 (i4_motion_type >= 7)))
 {
            i4_motion_type = 1;
 }

 }
 /*-----------------------------------------------------------------------*/
 /* dct type                                                              */
 /*-----------------------------------------------------------------------*/
 {
 if((u2_mb_type & MB_CODED) && ps_dec->u2_read_dct_type)
 {
            ps_dec->u2_field_dct = BIT((UWORD16)u4_next_word,15);
            u2_total_len += MB_DCT_TYPE_LEN;
            u4_next_word = (UWORD16)LSW((UWORD16)u4_next_word << MB_DCT_TYPE_LEN);
 }
 }
 /*-----------------------------------------------------------------------*/
 /* Quant scale code                                                      */
 /*-----------------------------------------------------------------------*/
 if(u2_mb_type & MB_QUANT)
 {
        UWORD16 u2_quant_scale_code;
        u2_quant_scale_code = BITS((UWORD16)u4_next_word,15,11);

        ps_dec->u1_quant_scale = (ps_dec->u2_q_scale_type) ?
            gau1_impeg2_non_linear_quant_scale[u2_quant_scale_code] : (u2_quant_scale_code << 1);
        u2_total_len += MB_QUANT_SCALE_CODE_LEN;
 }
    impeg2d_bit_stream_flush(ps_stream,u2_total_len);
 /*-----------------------------------------------------------------------*/
 /* Set the function pointers                                             */
 /*-----------------------------------------------------------------------*/
    ps_dec->u2_coded_mb    = (UWORD16)(u2_mb_type & MB_CODED);

 if(u2_mb_type & MB_BIDRECT)
 {
        UWORD16 u2_index       = (ps_dec->u2_motion_type);

        ps_dec->u2_prev_intra_mb    = 0;

         ps_dec->e_mb_pred         = BIDIRECT;
         ps_dec_mb_params = &ps_dec->ps_func_bi_direct[u2_index];
         ps_dec->s_mb_type = ps_dec_mb_params->s_mb_type;
         ps_dec_mb_params->pf_func_mb_params(ps_dec);
     }
     else if(u2_mb_type & MB_FORW_OR_BACK)
 {

        UWORD16 u2_refPic      = !(u2_mb_type & MB_MV_FORW);
        UWORD16 u2_index       = (ps_dec->u2_motion_type);
        ps_dec->u2_prev_intra_mb    = 0;

         ps_dec->e_mb_pred         = (e_pred_direction_t)u2_refPic;
         ps_dec_mb_params = &ps_dec->ps_func_forw_or_back[u2_index];
         ps_dec->s_mb_type = ps_dec_mb_params->s_mb_type;
         ps_dec_mb_params->pf_func_mb_params(ps_dec);
 
     }
 else if(u2_mb_type & MB_TYPE_INTRA)
 {
        ps_dec->u2_prev_intra_mb    = 1;
        impeg2d_dec_intra_mb(ps_dec);

 }
 else
 {
        ps_dec->u2_prev_intra_mb =0;
        ps_dec->e_mb_pred = FORW;
        ps_dec->u2_motion_type = 0;
        impeg2d_dec_0mv_coded_mb(ps_dec);
 }

 /*-----------------------------------------------------------------------*/
 /* decode cbp                                                            */
 /*-----------------------------------------------------------------------*/
 if((u2_mb_type & MB_TYPE_INTRA))
 {
        ps_dec->u2_cbp  = 0x3f;
        ps_dec->u2_prev_intra_mb    = 1;
 }
 else
 {
        ps_dec->u2_prev_intra_mb  = 0;
        ps_dec->u2_def_dc_pred[Y_LUMA] = 128 << ps_dec->u2_intra_dc_precision;
        ps_dec->u2_def_dc_pred[U_CHROMA] = 128 << ps_dec->u2_intra_dc_precision;
        ps_dec->u2_def_dc_pred[V_CHROMA] = 128 << ps_dec->u2_intra_dc_precision;
 if((ps_dec->u2_coded_mb))
 {
            UWORD16 cbpValue;
            cbpValue  = gau2_impeg2d_cbp_code[impeg2d_bit_stream_nxt(ps_stream,MB_CBP_LEN)];
            ps_dec->u2_cbp  = cbpValue & 0xFF;
            impeg2d_bit_stream_flush(ps_stream,(cbpValue >> 8) & 0x0FF);
 }
 else
 {

             ps_dec->u2_cbp  = 0;
         }
     }
 }
