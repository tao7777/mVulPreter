WORD32 ih264d_parse_islice(dec_struct_t *ps_dec,
                            UWORD16 u2_first_mb_in_slice)
{
 dec_pic_params_t * ps_pps = ps_dec->ps_cur_pps;
 dec_slice_params_t * ps_slice = ps_dec->ps_cur_slice;
    UWORD32 *pu4_bitstrm_buf = ps_dec->ps_bitstrm->pu4_buffer;
    UWORD32 *pu4_bitstrm_ofst = &ps_dec->ps_bitstrm->u4_ofst;
    UWORD32 u4_temp;
    WORD32 i_temp;
    WORD32 ret;

 /*--------------------------------------------------------------------*/
 /* Read remaining contents of the slice header                        */
 /*--------------------------------------------------------------------*/
 /* dec_ref_pic_marking function */
 /* G050 */

     if(ps_slice->u1_nal_ref_idc != 0)
     {
         if(!ps_dec->ps_dpb_cmds->u1_dpb_commands_read)
        {
            i_temp = ih264d_read_mmco_commands(ps_dec);
            if (i_temp < 0)
            {
                return ERROR_DBP_MANAGER_T;
            }
            ps_dec->u4_bitoffset = i_temp;
        }
         else
             ps_dec->ps_bitstrm->u4_ofst += ps_dec->u4_bitoffset;
     }
 /* G050 */

 /* Read slice_qp_delta */
    i_temp = ps_pps->u1_pic_init_qp
 + ih264d_sev(pu4_bitstrm_ofst, pu4_bitstrm_buf);
 if((i_temp < 0) || (i_temp > 51))
 return ERROR_INV_RANGE_QP_T;
    ps_slice->u1_slice_qp = i_temp;
    COPYTHECONTEXT("SH: slice_qp_delta",
                    ps_slice->u1_slice_qp - ps_pps->u1_pic_init_qp);

 if(ps_pps->u1_deblocking_filter_parameters_present_flag == 1)
 {
        u4_temp = ih264d_uev(pu4_bitstrm_ofst, pu4_bitstrm_buf);
        COPYTHECONTEXT("SH: disable_deblocking_filter_idc", u4_temp);

 if(u4_temp > SLICE_BOUNDARY_DBLK_DISABLED)
 {
 return ERROR_INV_SLICE_HDR_T;
 }
        ps_slice->u1_disable_dblk_filter_idc = u4_temp;
 if(u4_temp != 1)
 {
            i_temp = ih264d_sev(pu4_bitstrm_ofst, pu4_bitstrm_buf)
 << 1;
 if((MIN_DBLK_FIL_OFF > i_temp) || (i_temp > MAX_DBLK_FIL_OFF))
 {
 return ERROR_INV_SLICE_HDR_T;
 }
            ps_slice->i1_slice_alpha_c0_offset = i_temp;
            COPYTHECONTEXT("SH: slice_alpha_c0_offset_div2",
                            ps_slice->i1_slice_alpha_c0_offset >> 1);

            i_temp = ih264d_sev(pu4_bitstrm_ofst, pu4_bitstrm_buf)
 << 1;
 if((MIN_DBLK_FIL_OFF > i_temp) || (i_temp > MAX_DBLK_FIL_OFF))
 {
 return ERROR_INV_SLICE_HDR_T;
 }
            ps_slice->i1_slice_beta_offset = i_temp;
            COPYTHECONTEXT("SH: slice_beta_offset_div2",
                            ps_slice->i1_slice_beta_offset >> 1);

 }
 else
 {
            ps_slice->i1_slice_alpha_c0_offset = 0;
            ps_slice->i1_slice_beta_offset = 0;
 }
 }
 else
 {
        ps_slice->u1_disable_dblk_filter_idc = 0;
        ps_slice->i1_slice_alpha_c0_offset = 0;
        ps_slice->i1_slice_beta_offset = 0;
 }

 /* Initialization to check if number of motion vector per 2 Mbs */
 /* are exceeding the range or not */
    ps_dec->u2_mv_2mb[0] = 0;
    ps_dec->u2_mv_2mb[1] = 0;


 /*set slice header cone to 2 ,to indicate  correct header*/
    ps_dec->u1_slice_header_done = 2;

 if(ps_pps->u1_entropy_coding_mode)
 {
        SWITCHOFFTRACE; SWITCHONTRACECABAC;
 if(ps_dec->ps_cur_slice->u1_mbaff_frame_flag)
 {
            ps_dec->pf_get_mb_info = ih264d_get_mb_info_cabac_mbaff;
 }
 else
            ps_dec->pf_get_mb_info = ih264d_get_mb_info_cabac_nonmbaff;

        ret = ih264d_parse_islice_data_cabac(ps_dec, ps_slice,
                                             u2_first_mb_in_slice);
 if(ret != OK)
 return ret;
        SWITCHONTRACE; SWITCHOFFTRACECABAC;
 }
 else
 {
 if(ps_dec->ps_cur_slice->u1_mbaff_frame_flag)
 {
            ps_dec->pf_get_mb_info = ih264d_get_mb_info_cavlc_mbaff;
 }
 else
            ps_dec->pf_get_mb_info = ih264d_get_mb_info_cavlc_nonmbaff;
        ret = ih264d_parse_islice_data_cavlc(ps_dec, ps_slice,
                                       u2_first_mb_in_slice);
 if(ret != OK)
 return ret;
 }

 return OK;
}
