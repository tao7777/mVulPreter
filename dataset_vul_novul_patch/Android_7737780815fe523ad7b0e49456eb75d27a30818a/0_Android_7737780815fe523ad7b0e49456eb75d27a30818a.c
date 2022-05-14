IMPEG2D_ERROR_CODES_T impeg2d_dec_p_b_slice(dec_state_t *ps_dec)
{
    WORD16 *pi2_vld_out;
    UWORD32 i;
 yuv_buf_t *ps_cur_frm_buf      = &ps_dec->s_cur_frm_buf;

    UWORD32 u4_frm_offset          = 0;
 const dec_mb_params_t *ps_dec_mb_params;
    IMPEG2D_ERROR_CODES_T e_error   = (IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE;

    pi2_vld_out = ps_dec->ai2_vld_buf;
    memset(ps_dec->ai2_pred_mv,0,sizeof(ps_dec->ai2_pred_mv));

    ps_dec->u2_prev_intra_mb    = 0;
    ps_dec->u2_first_mb       = 1;

    ps_dec->u2_picture_width = ps_dec->u2_frame_width;

 if(ps_dec->u2_picture_structure != FRAME_PICTURE)
 {
        ps_dec->u2_picture_width <<= 1;
 if(ps_dec->u2_picture_structure == BOTTOM_FIELD)
 {
            u4_frm_offset = ps_dec->u2_frame_width;
 }
 }

 do
 {
        UWORD32 u4_x_offset, u4_y_offset;
        WORD32 ret;


        UWORD32 u4_x_dst_offset = 0;
        UWORD32 u4_y_dst_offset = 0;
        UWORD8  *pu1_out_p;
        UWORD8  *pu1_pred;
        WORD32 u4_pred_strd;

        IMPEG2D_TRACE_MB_START(ps_dec->u2_mb_x, ps_dec->u2_mb_y);

 if(ps_dec->e_pic_type == B_PIC)
            ret = impeg2d_dec_pnb_mb_params(ps_dec);
 else
            ret = impeg2d_dec_p_mb_params(ps_dec);

 
         if(ret)
             return IMPEG2D_MB_TEX_DECODE_ERR;

        if(0 >= ps_dec->u2_num_mbs_left)
        {
            break;
        }

         IMPEG2D_TRACE_MB_START(ps_dec->u2_mb_x, ps_dec->u2_mb_y);
 
         u4_x_dst_offset = u4_frm_offset + (ps_dec->u2_mb_x << 4);
        u4_y_dst_offset = (ps_dec->u2_mb_y << 4) * ps_dec->u2_picture_width;
        pu1_out_p = ps_cur_frm_buf->pu1_y + u4_x_dst_offset + u4_y_dst_offset;
 if(ps_dec->u2_prev_intra_mb == 0)
 {
            UWORD32 offset_x, offset_y, stride;
            UWORD16 index = (ps_dec->u2_motion_type);
 /*only for non intra mb's*/
 if(ps_dec->e_mb_pred == BIDIRECT)
 {
                ps_dec_mb_params = &ps_dec->ps_func_bi_direct[index];
 }
 else
 {
                ps_dec_mb_params = &ps_dec->ps_func_forw_or_back[index];
 }

            stride = ps_dec->u2_picture_width;

            offset_x = u4_frm_offset + (ps_dec->u2_mb_x << 4);

            offset_y = (ps_dec->u2_mb_y << 4);

            ps_dec->s_dest_buf.pu1_y = ps_cur_frm_buf->pu1_y + offset_y * stride + offset_x;

            stride = stride >> 1;

            ps_dec->s_dest_buf.pu1_u = ps_cur_frm_buf->pu1_u + (offset_y >> 1) * stride
 + (offset_x >> 1);

            ps_dec->s_dest_buf.pu1_v = ps_cur_frm_buf->pu1_v + (offset_y >> 1) * stride
 + (offset_x >> 1);

            PROFILE_DISABLE_MC_IF0
            ps_dec_mb_params->pf_mc(ps_dec);

 }
 for(i = 0; i < NUM_LUMA_BLKS; ++i)
 {
 if((ps_dec->u2_cbp & (1 << (BLOCKS_IN_MB - 1 - i))) != 0)
 {
                e_error = ps_dec->pf_vld_inv_quant(ps_dec, pi2_vld_out, ps_dec->pu1_inv_scan_matrix,
                              ps_dec->u2_prev_intra_mb, Y_LUMA, 0);
 if ((IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE != e_error)
 {
 return e_error;
 }

                u4_x_offset = gai2_impeg2_blk_x_off[i];

 if(ps_dec->u2_field_dct == 0)
                    u4_y_offset = gai2_impeg2_blk_y_off_frm[i] ;
 else
                    u4_y_offset = gai2_impeg2_blk_y_off_fld[i] ;





                IMPEG2D_IDCT_INP_STATISTICS(pi2_vld_out, ps_dec->u4_non_zero_cols, ps_dec->u4_non_zero_rows);

                PROFILE_DISABLE_IDCT_IF0
 {
                    WORD32 idx;
 if(1 == (ps_dec->u4_non_zero_cols | ps_dec->u4_non_zero_rows))
                        idx = 0;
 else
                        idx = 1;

 if(0 == ps_dec->u2_prev_intra_mb)
 {
                        pu1_pred = pu1_out_p + u4_y_offset * ps_dec->u2_picture_width + u4_x_offset;
                        u4_pred_strd = ps_dec->u2_picture_width << ps_dec->u2_field_dct;
 }
 else
 {
                        pu1_pred = (UWORD8 *)gau1_impeg2_zerobuf;
                        u4_pred_strd = 8;
 }

                    ps_dec->pf_idct_recon[idx * 2 + ps_dec->i4_last_value_one](pi2_vld_out,
                                                            ps_dec->ai2_idct_stg1,
                                                            pu1_pred,
                                                            pu1_out_p + u4_y_offset * ps_dec->u2_picture_width + u4_x_offset,
 8,
                                                            u4_pred_strd,
                                                            ps_dec->u2_picture_width << ps_dec->u2_field_dct,
 ~ps_dec->u4_non_zero_cols, ~ps_dec->u4_non_zero_rows);
 }
 }

 }

 /* For U and V blocks, divide the x and y offsets by 2. */
        u4_x_dst_offset >>= 1;
        u4_y_dst_offset >>= 2;


 /* In case of chrominance blocks the DCT will be frame DCT */
 /* i = 0, U component and i = 1 is V componet */
 if((ps_dec->u2_cbp & 0x02) != 0)
 {
            pu1_out_p = ps_cur_frm_buf->pu1_u + u4_x_dst_offset + u4_y_dst_offset;
            e_error = ps_dec->pf_vld_inv_quant(ps_dec, pi2_vld_out, ps_dec->pu1_inv_scan_matrix,
                          ps_dec->u2_prev_intra_mb, U_CHROMA, 0);
 if ((IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE != e_error)
 {
 return e_error;
 }


            IMPEG2D_IDCT_INP_STATISTICS(pi2_vld_out, ps_dec->u4_non_zero_cols, ps_dec->u4_non_zero_rows);

            PROFILE_DISABLE_IDCT_IF0
 {
                WORD32 idx;
 if(1 == (ps_dec->u4_non_zero_cols | ps_dec->u4_non_zero_rows))
                    idx = 0;
 else
                    idx = 1;

 if(0 == ps_dec->u2_prev_intra_mb)
 {
                    pu1_pred = pu1_out_p;
                    u4_pred_strd = ps_dec->u2_picture_width >> 1;
 }
 else
 {
                    pu1_pred = (UWORD8 *)gau1_impeg2_zerobuf;
                    u4_pred_strd = 8;
 }

                ps_dec->pf_idct_recon[idx * 2 + ps_dec->i4_last_value_one](pi2_vld_out,
                                                        ps_dec->ai2_idct_stg1,
                                                        pu1_pred,
                                                        pu1_out_p,
 8,
                                                        u4_pred_strd,
                                                        ps_dec->u2_picture_width >> 1,
 ~ps_dec->u4_non_zero_cols, ~ps_dec->u4_non_zero_rows);

 }

 }


 if((ps_dec->u2_cbp & 0x01) != 0)
 {
            pu1_out_p = ps_cur_frm_buf->pu1_v + u4_x_dst_offset + u4_y_dst_offset;
            e_error = ps_dec->pf_vld_inv_quant(ps_dec, pi2_vld_out, ps_dec->pu1_inv_scan_matrix,
                          ps_dec->u2_prev_intra_mb, V_CHROMA, 0);
 if ((IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE != e_error)
 {
 return e_error;
 }


            IMPEG2D_IDCT_INP_STATISTICS(pi2_vld_out, ps_dec->u4_non_zero_cols, ps_dec->u4_non_zero_rows);

            PROFILE_DISABLE_IDCT_IF0
 {
                WORD32 idx;
 if(1 == (ps_dec->u4_non_zero_cols | ps_dec->u4_non_zero_rows))
                    idx = 0;
 else
                    idx = 1;
 if(0 == ps_dec->u2_prev_intra_mb)
 {
                    pu1_pred = pu1_out_p;
                    u4_pred_strd = ps_dec->u2_picture_width >> 1;
 }
 else
 {
                    pu1_pred = (UWORD8 *)gau1_impeg2_zerobuf;
                    u4_pred_strd = 8;
 }

                ps_dec->pf_idct_recon[idx * 2 + ps_dec->i4_last_value_one](pi2_vld_out,
                                                        ps_dec->ai2_idct_stg1,
                                                        pu1_pred,
                                                        pu1_out_p,
 8,
                                                        u4_pred_strd,
                                                        ps_dec->u2_picture_width >> 1,
 ~ps_dec->u4_non_zero_cols, ~ps_dec->u4_non_zero_rows);

 }
 }

        ps_dec->u2_num_mbs_left--;
        ps_dec->u2_first_mb = 0;
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
 }
 while(ps_dec->u2_num_mbs_left != 0 && impeg2d_bit_stream_nxt(&ps_dec->s_bit_stream,23) != 0x0);
 return e_error;
}
