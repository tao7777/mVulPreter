WORD32 ih264d_read_mmco_commands(struct _DecStruct * ps_dec)
{
 dec_bit_stream_t *ps_bitstrm = ps_dec->ps_bitstrm;
 dpb_commands_t *ps_dpb_cmds = ps_dec->ps_dpb_cmds;
 dec_slice_params_t * ps_slice = ps_dec->ps_cur_slice;
    WORD32 j;
    UWORD8 u1_buf_mode;
 struct MMCParams *ps_mmc_params;
    UWORD32 *pu4_bitstrm_buf = ps_dec->ps_bitstrm->pu4_buffer;
    UWORD32 *pu4_bitstrm_ofst = &ps_bitstrm->u4_ofst;
    UWORD32 u4_bit_ofst = ps_dec->ps_bitstrm->u4_ofst;

    ps_slice->u1_mmco_equalto5 = 0;
 {
 if(ps_dec->u1_nal_unit_type == IDR_SLICE_NAL)
 {
            ps_slice->u1_no_output_of_prior_pics_flag =
                            ih264d_get_bit_h264(ps_bitstrm);
            COPYTHECONTEXT("SH: no_output_of_prior_pics_flag",
                            ps_slice->u1_no_output_of_prior_pics_flag);
            ps_slice->u1_long_term_reference_flag = ih264d_get_bit_h264(
                            ps_bitstrm);
            COPYTHECONTEXT("SH: long_term_reference_flag",
                            ps_slice->u1_long_term_reference_flag);
            ps_dpb_cmds->u1_idr_pic = 1;
            ps_dpb_cmds->u1_no_output_of_prior_pics_flag =
                            ps_slice->u1_no_output_of_prior_pics_flag;
            ps_dpb_cmds->u1_long_term_reference_flag =
                            ps_slice->u1_long_term_reference_flag;
 }
 else
 {
            u1_buf_mode = ih264d_get_bit_h264(ps_bitstrm); //0 - sliding window; 1 - arbitrary
            COPYTHECONTEXT("SH: adaptive_ref_pic_buffering_flag", u1_buf_mode);
            ps_dpb_cmds->u1_buf_mode = u1_buf_mode;
            j = 0;

 if(u1_buf_mode == 1)
 {
                UWORD32 u4_mmco;
                UWORD32 u4_diff_pic_num;
                UWORD32 u4_lt_idx, u4_max_lt_idx;

                u4_mmco = ih264d_uev(pu4_bitstrm_ofst,

                                      pu4_bitstrm_buf);
                 while(u4_mmco != END_OF_MMCO)
                 {
                     ps_mmc_params = &ps_dpb_cmds->as_mmc_params[j];
                     ps_mmc_params->u4_mmco = u4_mmco;
                     switch(u4_mmco)
 {
 case MARK_ST_PICNUM_AS_NONREF:
                            u4_diff_pic_num = ih264d_uev(pu4_bitstrm_ofst,
                                                         pu4_bitstrm_buf);
                            ps_mmc_params->u4_diff_pic_num = u4_diff_pic_num;
 break;

 case MARK_LT_INDEX_AS_NONREF:
                            u4_lt_idx = ih264d_uev(pu4_bitstrm_ofst,
                                                   pu4_bitstrm_buf);
                            ps_mmc_params->u4_lt_idx = u4_lt_idx;
 break;

 case MARK_ST_PICNUM_AS_LT_INDEX:
                            u4_diff_pic_num = ih264d_uev(pu4_bitstrm_ofst,
                                                         pu4_bitstrm_buf);
                            ps_mmc_params->u4_diff_pic_num = u4_diff_pic_num;
                            u4_lt_idx = ih264d_uev(pu4_bitstrm_ofst,
                                                   pu4_bitstrm_buf);
                            ps_mmc_params->u4_lt_idx = u4_lt_idx;
 break;

 case SET_MAX_LT_INDEX:
 {
                            u4_max_lt_idx = ih264d_uev(pu4_bitstrm_ofst,
                                                       pu4_bitstrm_buf);
                            ps_mmc_params->u4_max_lt_idx_plus1 = u4_max_lt_idx;
 break;
 }
 case RESET_REF_PICTURES:
 {
                            ps_slice->u1_mmco_equalto5 = 1;
 break;
 }

 case SET_LT_INDEX:
                            u4_lt_idx = ih264d_uev(pu4_bitstrm_ofst,
                                                   pu4_bitstrm_buf);
                            ps_mmc_params->u4_lt_idx = u4_lt_idx;
 break;

 default:
 break;
 }
                    u4_mmco = ih264d_uev(pu4_bitstrm_ofst,
                                         pu4_bitstrm_buf);

                    j++;
 }
                ps_dpb_cmds->u1_num_of_commands = j;

 }
 }
        ps_dpb_cmds->u1_dpb_commands_read = 1;
        ps_dpb_cmds->u1_dpb_commands_read_slc = 1;

 }
    u4_bit_ofst = ps_dec->ps_bitstrm->u4_ofst - u4_bit_ofst;
 return u4_bit_ofst;
}
