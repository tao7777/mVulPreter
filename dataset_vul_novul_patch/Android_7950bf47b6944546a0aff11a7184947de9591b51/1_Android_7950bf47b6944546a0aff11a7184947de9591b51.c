WORD32 ih264d_parse_islice_data_cavlc(dec_struct_t * ps_dec,
 dec_slice_params_t * ps_slice,
                                      UWORD16 u2_first_mb_in_slice)
{
    UWORD8 uc_more_data_flag;
    UWORD8 u1_num_mbs, u1_mb_idx;
 dec_mb_info_t *ps_cur_mb_info;
 deblk_mb_t *ps_cur_deblk_mb;
 dec_bit_stream_t * const ps_bitstrm = ps_dec->ps_bitstrm;
    UWORD32 *pu4_bitstrm_ofst = &ps_bitstrm->u4_ofst;
    UWORD32 *pu4_bitstrm_buf = ps_bitstrm->pu4_buffer;
    UWORD16 i2_pic_wdin_mbs = ps_dec->u2_frm_wd_in_mbs;
    WORD16 i2_cur_mb_addr;
    UWORD8 u1_mbaff;
    UWORD8 u1_num_mbs_next, u1_end_of_row, u1_tfr_n_mb;
    WORD32 ret = OK;

    ps_dec->u1_qp = ps_slice->u1_slice_qp;
    ih264d_update_qp(ps_dec, 0);
    u1_mbaff = ps_slice->u1_mbaff_frame_flag;

 /* initializations */
    u1_mb_idx = ps_dec->u1_mb_idx;
    u1_num_mbs = u1_mb_idx;

    uc_more_data_flag = 1;
    i2_cur_mb_addr = u2_first_mb_in_slice << u1_mbaff;

 do
 {
        UWORD8 u1_mb_type;

        ps_dec->pv_prev_mb_parse_tu_coeff_data = ps_dec->pv_parse_tu_coeff_data;

 if(i2_cur_mb_addr > ps_dec->ps_cur_sps->u2_max_mb_addr)
 {
            ret = ERROR_MB_ADDRESS_T;
 break;
 }

        ps_cur_mb_info = ps_dec->ps_nmb_info + u1_num_mbs;
        ps_dec->u4_num_mbs_cur_nmb = u1_num_mbs;
        ps_dec->u4_num_pmbair = (u1_num_mbs >> u1_mbaff);

        ps_cur_mb_info->u1_end_of_slice = 0;

 /***************************************************************/
 /* Get the required information for decoding of MB             */
 /* mb_x, mb_y , neighbour availablity,                         */
 /***************************************************************/
        ps_dec->pf_get_mb_info(ps_dec, i2_cur_mb_addr, ps_cur_mb_info, 0);

 /***************************************************************/
 /* Set the deblocking parameters for this MB                   */
 /***************************************************************/
        ps_cur_deblk_mb = ps_dec->ps_deblk_mbn + u1_num_mbs;

 if(ps_dec->u4_app_disable_deblk_frm == 0)
            ih264d_set_deblocking_parameters(ps_cur_deblk_mb, ps_slice,
                                             ps_dec->u1_mb_ngbr_availablity,
                                             ps_dec->u1_cur_mb_fld_dec_flag);

        ps_cur_deblk_mb->u1_mb_type = ps_cur_deblk_mb->u1_mb_type | D_INTRA_MB;

 /**************************************************************/
 /* Macroblock Layer Begins, Decode the u1_mb_type                */
 /**************************************************************/
 {
            UWORD32 u4_bitstream_offset = *pu4_bitstrm_ofst;
            UWORD32 u4_word, u4_ldz, u4_temp;

 /***************************************************************/
 /* Find leading zeros in next 32 bits                          */
 /***************************************************************/
            NEXTBITS_32(u4_word, u4_bitstream_offset, pu4_bitstrm_buf);
            u4_ldz = CLZ(u4_word);
 /* Flush the ps_bitstrm */
            u4_bitstream_offset += (u4_ldz + 1);
 /* Read the suffix from the ps_bitstrm */
            u4_word = 0;
 if(u4_ldz)
                GETBITS(u4_word, u4_bitstream_offset, pu4_bitstrm_buf,
                        u4_ldz);
 *pu4_bitstrm_ofst = u4_bitstream_offset;
            u4_temp = ((1 << u4_ldz) + u4_word - 1);
 if(u4_temp > 25)
 return ERROR_MB_TYPE;
            u1_mb_type = u4_temp;

 }
        ps_cur_mb_info->u1_mb_type = u1_mb_type;
        COPYTHECONTEXT("u1_mb_type", u1_mb_type);

 /**************************************************************/
 /* Parse Macroblock data                                      */
 /**************************************************************/
 if(25 == u1_mb_type)
 {
 /* I_PCM_MB */
            ps_cur_mb_info->ps_curmb->u1_mb_type = I_PCM_MB;
            ret = ih264d_parse_ipcm_mb(ps_dec, ps_cur_mb_info, u1_num_mbs);
 if(ret != OK)
 return ret;
            ps_cur_deblk_mb->u1_mb_qp = 0;
 }
 else
 {
            ret = ih264d_parse_imb_cavlc(ps_dec, ps_cur_mb_info, u1_num_mbs, u1_mb_type);
 if(ret != OK)
 return ret;
            ps_cur_deblk_mb->u1_mb_qp = ps_dec->u1_qp;
 }


         if(u1_mbaff)
         {
             ih264d_update_mbaff_left_nnz(ps_dec, ps_cur_mb_info);
         }
         /**************************************************************/
         /* Get next Macroblock address                                */
 /**************************************************************/

        i2_cur_mb_addr++;
        uc_more_data_flag = MORE_RBSP_DATA(ps_bitstrm);

 /* Store the colocated information */
 {
 mv_pred_t *ps_mv_nmb_start = ps_dec->ps_mv_cur + (u1_num_mbs << 4);

 mv_pred_t s_mvPred =
 {
 { 0, 0, 0, 0 },
 { -1, -1 }, 0, 0};
            ih264d_rep_mv_colz(ps_dec, &s_mvPred, ps_mv_nmb_start, 0,
 (UWORD8)(ps_dec->u1_cur_mb_fld_dec_flag << 1), 4,
 4);
 }

 /*if num _cores is set to 3,compute bs will be done in another thread*/
 if(ps_dec->u4_num_cores < 3)
 {
 if(ps_dec->u4_app_disable_deblk_frm == 0)
                ps_dec->pf_compute_bs(ps_dec, ps_cur_mb_info,
 (UWORD16)(u1_num_mbs >> u1_mbaff));
 }
        u1_num_mbs++;

 /****************************************************************/
 /* Check for End Of Row                                         */
 /****************************************************************/
        u1_num_mbs_next = i2_pic_wdin_mbs - ps_dec->u2_mbx - 1;
        u1_end_of_row = (!u1_num_mbs_next) && (!(u1_mbaff && (u1_num_mbs & 0x01)));
        u1_tfr_n_mb = (u1_num_mbs == ps_dec->u1_recon_mb_grp) || u1_end_of_row
 || (!uc_more_data_flag);
        ps_cur_mb_info->u1_end_of_slice = (!uc_more_data_flag);

 /*H264_DEC_DEBUG_PRINT("Pic: %d Mb_X=%d Mb_Y=%d",
         ps_slice->i4_poc >> ps_slice->u1_field_pic_flag,
         ps_dec->u2_mbx,ps_dec->u2_mby + (1 - ps_cur_mb_info->u1_topmb));
         H264_DEC_DEBUG_PRINT("u1_tfr_n_mb || (!uc_more_data_flag): %d", u1_tfr_n_mb || (!uc_more_data_flag));*/
 if(u1_tfr_n_mb || (!uc_more_data_flag))
 {

 if(ps_dec->u1_separate_parse)
 {
                ih264d_parse_tfr_nmb(ps_dec, u1_mb_idx, u1_num_mbs,
                                     u1_num_mbs_next, u1_tfr_n_mb, u1_end_of_row);
                ps_dec->ps_nmb_info +=  u1_num_mbs;
 }
 else
 {
                ih264d_decode_recon_tfr_nmb(ps_dec, u1_mb_idx, u1_num_mbs,
                                            u1_num_mbs_next, u1_tfr_n_mb,
                                            u1_end_of_row);
 }
            ps_dec->u2_total_mbs_coded += u1_num_mbs;
 if(u1_tfr_n_mb)
                u1_num_mbs = 0;
            u1_mb_idx = u1_num_mbs;
            ps_dec->u1_mb_idx = u1_num_mbs;

 }
 }
 while(uc_more_data_flag);

    ps_dec->u4_num_mbs_cur_nmb = 0;
    ps_dec->ps_cur_slice->u4_mbs_in_slice = i2_cur_mb_addr

 - (u2_first_mb_in_slice << u1_mbaff);

 return ret;
}
