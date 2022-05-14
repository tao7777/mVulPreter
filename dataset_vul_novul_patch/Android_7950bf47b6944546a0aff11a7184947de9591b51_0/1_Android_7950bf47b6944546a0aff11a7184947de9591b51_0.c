WORD32 ih264d_parse_inter_slice_data_cavlc(dec_struct_t * ps_dec,
 dec_slice_params_t * ps_slice,
                                           UWORD16 u2_first_mb_in_slice)
{
    UWORD32 uc_more_data_flag;
    WORD32 i2_cur_mb_addr;
    UWORD32 u1_num_mbs, u1_num_mbsNby2, u1_mb_idx;
    UWORD32 i2_mb_skip_run;
    UWORD32 u1_read_mb_type;

    UWORD32 u1_mbaff;
    UWORD32 u1_num_mbs_next, u1_end_of_row;
 const UWORD32 i2_pic_wdin_mbs = ps_dec->u2_frm_wd_in_mbs;
    UWORD32 u1_slice_end = 0;
    UWORD32 u1_tfr_n_mb = 0;
    UWORD32 u1_decode_nmb = 0;

 dec_bit_stream_t * const ps_bitstrm = ps_dec->ps_bitstrm;
    UWORD32 *pu4_bitstrm_buf = ps_bitstrm->pu4_buffer;
    UWORD32 *pu4_bitstrm_ofst = &ps_bitstrm->u4_ofst;
 deblk_mb_t *ps_cur_deblk_mb;
 dec_mb_info_t *ps_cur_mb_info;
 parse_pmbarams_t *ps_parse_mb_data = ps_dec->ps_parse_mb_data;
    UWORD32 u1_inter_mb_type;
    UWORD32 u1_deblk_mb_type;
    UWORD32 u1_mb_threshold;
    WORD32 ret = OK;

 /******************************************************/
 /* Initialisations specific to B or P slice           */
 /******************************************************/

 if(ps_slice->u1_slice_type == P_SLICE)
 {
        u1_inter_mb_type = P_MB;
        u1_deblk_mb_type = D_INTER_MB;
        u1_mb_threshold = 5;
 }
 else // B_SLICE
 {
        u1_inter_mb_type = B_MB;
        u1_deblk_mb_type = D_B_SLICE;
        u1_mb_threshold = 23;
 }
 /******************************************************/
 /* Slice Level Initialisations                        */
 /******************************************************/
    ps_dec->u1_qp = ps_slice->u1_slice_qp;
    ih264d_update_qp(ps_dec, 0);
    u1_mb_idx = ps_dec->u1_mb_idx;
    u1_num_mbs = u1_mb_idx;

    u1_num_mbsNby2 = 0;
    u1_mbaff = ps_slice->u1_mbaff_frame_flag;
    i2_cur_mb_addr = u2_first_mb_in_slice << u1_mbaff;
    i2_mb_skip_run = 0;
    uc_more_data_flag = 1;
    u1_read_mb_type = 0;

 while(!u1_slice_end)
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

        ps_cur_mb_info->u1_Mux = 0;
        ps_dec->u4_num_pmbair = (u1_num_mbs >> u1_mbaff);
        ps_cur_deblk_mb = ps_dec->ps_deblk_mbn + u1_num_mbs;

        ps_cur_mb_info->u1_end_of_slice = 0;

 /* Storing Default partition info */
        ps_parse_mb_data->u1_num_part = 1;
        ps_parse_mb_data->u1_isI_mb = 0;

 if((!i2_mb_skip_run) && (!u1_read_mb_type))
 {

            UWORD32 u4_bitstream_offset = *pu4_bitstrm_ofst;
            UWORD32 u4_word, u4_ldz;

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
 {
                GETBITS(u4_word, u4_bitstream_offset, pu4_bitstrm_buf,
                        u4_ldz);
 }
 *pu4_bitstrm_ofst = u4_bitstream_offset;
            i2_mb_skip_run = ((1 << u4_ldz) + u4_word - 1);
            COPYTHECONTEXT("mb_skip_run", i2_mb_skip_run);
            uc_more_data_flag = MORE_RBSP_DATA(ps_bitstrm);
            u1_read_mb_type = uc_more_data_flag;
 }

 /***************************************************************/
 /* Get the required information for decoding of MB                  */
 /* mb_x, mb_y , neighbour availablity,                              */
 /***************************************************************/
        ps_dec->pf_get_mb_info(ps_dec, i2_cur_mb_addr, ps_cur_mb_info, i2_mb_skip_run);

 /***************************************************************/
 /* Set the deblocking parameters for this MB                   */
 /***************************************************************/
 if(ps_dec->u4_app_disable_deblk_frm == 0)
            ih264d_set_deblocking_parameters(ps_cur_deblk_mb, ps_slice,
                                             ps_dec->u1_mb_ngbr_availablity,
                                             ps_dec->u1_cur_mb_fld_dec_flag);

 if(i2_mb_skip_run)
 {
 /* Set appropriate flags in ps_cur_mb_info and ps_dec */
            ps_dec->i1_prev_mb_qp_delta = 0;
            ps_dec->u1_sub_mb_num = 0;
            ps_cur_mb_info->u1_mb_type = MB_SKIP;
            ps_cur_mb_info->u1_mb_mc_mode = PRED_16x16;
            ps_cur_mb_info->u1_cbp = 0;

 {
 /* Storing Skip partition info */
 parse_part_params_t *ps_part_info = ps_dec->ps_part;
                ps_part_info->u1_is_direct = PART_DIRECT_16x16;
                ps_part_info->u1_sub_mb_num = 0;
                ps_dec->ps_part++;
 }

 /* Update Nnzs */
            ih264d_update_nnz_for_skipmb(ps_dec, ps_cur_mb_info, CAVLC);

            ps_cur_mb_info->ps_curmb->u1_mb_type = u1_inter_mb_type;
            ps_cur_deblk_mb->u1_mb_type |= u1_deblk_mb_type;

            i2_mb_skip_run--;
 }
 else
 {
            u1_read_mb_type = 0;
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
 if(u4_temp > (UWORD32)(25 + u1_mb_threshold))
 return ERROR_MB_TYPE;
                u1_mb_type = u4_temp;
                COPYTHECONTEXT("u1_mb_type", u1_mb_type);
 }
            ps_cur_mb_info->u1_mb_type = u1_mb_type;

 /**************************************************************/
 /* Parse Macroblock data                                      */
 /**************************************************************/
 if(u1_mb_type < u1_mb_threshold)
 {
                ps_cur_mb_info->ps_curmb->u1_mb_type = u1_inter_mb_type;

                ret = ps_dec->pf_parse_inter_mb(ps_dec, ps_cur_mb_info, u1_num_mbs,
                                          u1_num_mbsNby2);
 if(ret != OK)
 return ret;
                ps_cur_deblk_mb->u1_mb_type |= u1_deblk_mb_type;
 }
 else
 {
 /* Storing Intra partition info */
                ps_parse_mb_data->u1_num_part = 0;
                ps_parse_mb_data->u1_isI_mb = 1;

 if((25 + u1_mb_threshold) == u1_mb_type)
 {
 /* I_PCM_MB */
                    ps_cur_mb_info->ps_curmb->u1_mb_type = I_PCM_MB;
                    ret = ih264d_parse_ipcm_mb(ps_dec, ps_cur_mb_info, u1_num_mbs);
 if(ret != OK)
 return ret;
                    ps_dec->u1_qp = 0;
 }
 else
 {
                    ret = ih264d_parse_imb_cavlc(
                                    ps_dec, ps_cur_mb_info, u1_num_mbs,
 (UWORD8)(u1_mb_type - u1_mb_threshold));
 if(ret != OK)
 return ret;
 }

                ps_cur_deblk_mb->u1_mb_type |= D_INTRA_MB;
 }
            uc_more_data_flag = MORE_RBSP_DATA(ps_bitstrm);
 }
        ps_cur_deblk_mb->u1_mb_qp = ps_dec->u1_qp;


         if(u1_mbaff)
         {
             ih264d_update_mbaff_left_nnz(ps_dec, ps_cur_mb_info);
         }
         /**************************************************************/
         /* Get next Macroblock address                                */
 /**************************************************************/
        i2_cur_mb_addr++;

        u1_num_mbs++;
        u1_num_mbsNby2++;
        ps_parse_mb_data++;

 /****************************************************************/
 /* Check for End Of Row and other flags that determine when to  */
 /* do DMA setup for N/2-Mb, Decode for N-Mb, and Transfer for   */
 /* N-Mb                                                         */
 /****************************************************************/
        u1_num_mbs_next = i2_pic_wdin_mbs - ps_dec->u2_mbx - 1;
        u1_end_of_row = (!u1_num_mbs_next) && (!(u1_mbaff && (u1_num_mbs & 0x01)));
        u1_slice_end = (!(uc_more_data_flag || i2_mb_skip_run));
        u1_tfr_n_mb = (u1_num_mbs == ps_dec->u1_recon_mb_grp) || u1_end_of_row
 || u1_slice_end;
        u1_decode_nmb = u1_tfr_n_mb || u1_slice_end;
        ps_cur_mb_info->u1_end_of_slice = u1_slice_end;

 /*u1_dma_nby2mb   = u1_decode_nmb ||
         (u1_num_mbsNby2 == ps_dec->u1_recon_mb_grp_pair);*/

 if(u1_decode_nmb)
 {
            ps_dec->pf_mvpred_ref_tfr_nby2mb(ps_dec, u1_mb_idx, u1_num_mbs);
            u1_num_mbsNby2 = 0;

 {
                ps_parse_mb_data = ps_dec->ps_parse_mb_data;
                ps_dec->ps_part = ps_dec->ps_parse_part_params;
 }
 }

 /*H264_DEC_DEBUG_PRINT("Pic: %d Mb_X=%d Mb_Y=%d",
         ps_slice->i4_poc >> ps_slice->u1_field_pic_flag,
         ps_dec->u2_mbx,ps_dec->u2_mby + (1 - ps_cur_mb_info->u1_topmb));
         H264_DEC_DEBUG_PRINT("u1_decode_nmb: %d", u1_decode_nmb);*/
 if(u1_decode_nmb)
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

    ps_dec->u4_num_mbs_cur_nmb = 0;
    ps_dec->ps_cur_slice->u4_mbs_in_slice = i2_cur_mb_addr
 - (u2_first_mb_in_slice << u1_mbaff);


 return ret;
}
