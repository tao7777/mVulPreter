WORD32 ih264d_parse_sps(dec_struct_t *ps_dec, dec_bit_stream_t *ps_bitstrm)

 {
     UWORD8 i;
     dec_seq_params_t *ps_seq = NULL;
    UWORD8 u1_profile_idc, u1_level_idc, u1_seq_parameter_set_id, u1_mb_aff_flag = 0;
     UWORD16 i2_max_frm_num;
     UWORD32 *pu4_bitstrm_buf = ps_bitstrm->pu4_buffer;
     UWORD32 *pu4_bitstrm_ofst = &ps_bitstrm->u4_ofst;
    UWORD8 u1_frm, uc_constraint_set0_flag, uc_constraint_set1_flag;
    WORD32 i4_cropped_ht, i4_cropped_wd;
    UWORD32 u4_temp;
    WORD32 pic_height_in_map_units_minus1 = 0;
    UWORD32 u2_pic_wd = 0;
    UWORD32 u2_pic_ht = 0;
    UWORD32 u2_frm_wd_y = 0;
    UWORD32 u2_frm_ht_y = 0;
    UWORD32 u2_frm_wd_uv = 0;
    UWORD32 u2_frm_ht_uv = 0;
    UWORD32 u2_crop_offset_y = 0;
    UWORD32 u2_crop_offset_uv = 0;
    WORD32 ret;
    UWORD32 u4_num_reorder_frames;
 /* High profile related syntax element */
    WORD32 i4_i;
 /* G050 */
    UWORD8 u1_frame_cropping_flag, u1_frame_cropping_rect_left_ofst,
                    u1_frame_cropping_rect_right_ofst,
                    u1_frame_cropping_rect_top_ofst,
                    u1_frame_cropping_rect_bottom_ofst;
 /* G050 */
 /*--------------------------------------------------------------------*/
 /* Decode seq_parameter_set_id and profile and level values           */
 /*--------------------------------------------------------------------*/
    SWITCHONTRACE;
    u1_profile_idc = ih264d_get_bits_h264(ps_bitstrm, 8);
    COPYTHECONTEXT("SPS: profile_idc",u1_profile_idc);

 /* G050 */
    uc_constraint_set0_flag = ih264d_get_bit_h264(ps_bitstrm);
    uc_constraint_set1_flag = ih264d_get_bit_h264(ps_bitstrm);
    ih264d_get_bit_h264(ps_bitstrm);

 /*****************************************************/
 /* Read 5 bits for uc_constraint_set3_flag (1 bit)   */
 /* and reserved_zero_4bits (4 bits) - Sushant        */
 /*****************************************************/
    ih264d_get_bits_h264(ps_bitstrm, 5);
 /* G050 */

 /* Check whether particular profile is suported or not */
 /* Check whether particular profile is suported or not */
 if((u1_profile_idc != MAIN_PROFILE_IDC) &&

 (u1_profile_idc != BASE_PROFILE_IDC) &&

 (u1_profile_idc != HIGH_PROFILE_IDC)

 )
 {

 /* Apart from Baseline, main and high profile,
         * only extended profile is supported provided
         * uc_constraint_set0_flag or uc_constraint_set1_flag are set to 1
         */
 if((u1_profile_idc != EXTENDED_PROFILE_IDC) ||
 ((uc_constraint_set1_flag != 1) && (uc_constraint_set0_flag != 1)))
 {
 return (ERROR_FEATURE_UNAVAIL);
 }
 }

    u1_level_idc = ih264d_get_bits_h264(ps_bitstrm, 8);



    COPYTHECONTEXT("SPS: u4_level_idc",u1_level_idc);

    u4_temp = ih264d_uev(pu4_bitstrm_ofst, pu4_bitstrm_buf);
 if(u4_temp & MASK_ERR_SEQ_SET_ID)
 return ERROR_INV_SPS_PPS_T;
    u1_seq_parameter_set_id = u4_temp;
    COPYTHECONTEXT("SPS: seq_parameter_set_id",
                    u1_seq_parameter_set_id);

 /*--------------------------------------------------------------------*/
 /* Find an seq param entry in seqparam array of decStruct             */
 /*--------------------------------------------------------------------*/

    ps_seq = ps_dec->pv_scratch_sps_pps;
 if(ps_dec->i4_header_decoded & 1)
 {
 *ps_seq = *ps_dec->ps_cur_sps;
 }


 if((ps_dec->i4_header_decoded & 1) && (ps_seq->u1_profile_idc != u1_profile_idc))
 {
        ps_dec->u1_res_changed = 1;
 return IVD_RES_CHANGED;
 }

 if((ps_dec->i4_header_decoded & 1) && (ps_seq->u1_level_idc != u1_level_idc))
 {
        ps_dec->u1_res_changed = 1;
 return IVD_RES_CHANGED;
 }

    ps_seq->u1_profile_idc = u1_profile_idc;
    ps_seq->u1_level_idc = u1_level_idc;
    ps_seq->u1_seq_parameter_set_id = u1_seq_parameter_set_id;

 /*******************************************************************/
 /* Initializations for high profile - Sushant                      */
 /*******************************************************************/
    ps_seq->i4_chroma_format_idc = 1;
    ps_seq->i4_bit_depth_luma_minus8 = 0;
    ps_seq->i4_bit_depth_chroma_minus8 = 0;
    ps_seq->i4_qpprime_y_zero_transform_bypass_flag = 0;
    ps_seq->i4_seq_scaling_matrix_present_flag = 0;
 if(u1_profile_idc == HIGH_PROFILE_IDC)
 {

 /* reading chroma_format_idc   */
        ps_seq->i4_chroma_format_idc = ih264d_uev(pu4_bitstrm_ofst,
                                                  pu4_bitstrm_buf);

 /* Monochrome is not supported */
 if(ps_seq->i4_chroma_format_idc != 1)
 {
 return ERROR_INV_SPS_PPS_T;
 }

 /* reading bit_depth_luma_minus8   */
        ps_seq->i4_bit_depth_luma_minus8 = ih264d_uev(pu4_bitstrm_ofst,
                                                      pu4_bitstrm_buf);

 if(ps_seq->i4_bit_depth_luma_minus8 != 0)
 {
 return ERROR_INV_SPS_PPS_T;
 }

 /* reading bit_depth_chroma_minus8   */
        ps_seq->i4_bit_depth_chroma_minus8 = ih264d_uev(pu4_bitstrm_ofst,
                                                        pu4_bitstrm_buf);

 if(ps_seq->i4_bit_depth_chroma_minus8 != 0)
 {
 return ERROR_INV_SPS_PPS_T;
 }

 /* reading qpprime_y_zero_transform_bypass_flag   */
        ps_seq->i4_qpprime_y_zero_transform_bypass_flag =
 (WORD32)ih264d_get_bit_h264(ps_bitstrm);

 if(ps_seq->i4_qpprime_y_zero_transform_bypass_flag != 0)
 {
 return ERROR_INV_SPS_PPS_T;
 }

 /* reading seq_scaling_matrix_present_flag   */
        ps_seq->i4_seq_scaling_matrix_present_flag =
 (WORD32)ih264d_get_bit_h264(ps_bitstrm);

 if(ps_seq->i4_seq_scaling_matrix_present_flag)
 {
 for(i4_i = 0; i4_i < 8; i4_i++)
 {
                ps_seq->u1_seq_scaling_list_present_flag[i4_i] =
                                ih264d_get_bit_h264(ps_bitstrm);

 /* initialize u1_use_default_scaling_matrix_flag[i4_i] to zero */
 /* before calling scaling list                             */
                ps_seq->u1_use_default_scaling_matrix_flag[i4_i] = 0;

 if(ps_seq->u1_seq_scaling_list_present_flag[i4_i])
 {
 if(i4_i < 6)
 {
                        ih264d_scaling_list(
                                        ps_seq->i2_scalinglist4x4[i4_i],
 16,
 &ps_seq->u1_use_default_scaling_matrix_flag[i4_i],
                                        ps_bitstrm);
 }
 else
 {
                        ih264d_scaling_list(
                                        ps_seq->i2_scalinglist8x8[i4_i - 6],
 64,
 &ps_seq->u1_use_default_scaling_matrix_flag[i4_i],
                                        ps_bitstrm);
 }
 }
 }
 }
 }
 /*--------------------------------------------------------------------*/
 /* Decode MaxFrameNum                                                 */
 /*--------------------------------------------------------------------*/
    u4_temp = 4 + ih264d_uev(pu4_bitstrm_ofst, pu4_bitstrm_buf);
 if(u4_temp > MAX_BITS_IN_FRAME_NUM)
 {
 return ERROR_INV_SPS_PPS_T;
 }
    ps_seq->u1_bits_in_frm_num = u4_temp;
    COPYTHECONTEXT("SPS: log2_max_frame_num_minus4",
 (ps_seq->u1_bits_in_frm_num - 4));

    i2_max_frm_num = (1 << (ps_seq->u1_bits_in_frm_num));
    ps_seq->u2_u4_max_pic_num_minus1 = i2_max_frm_num - 1;
 /*--------------------------------------------------------------------*/
 /* Decode picture order count and related values                      */
 /*--------------------------------------------------------------------*/
    u4_temp = ih264d_uev(pu4_bitstrm_ofst, pu4_bitstrm_buf);

 if(u4_temp > MAX_PIC_ORDER_CNT_TYPE)
 {
 return ERROR_INV_POC_TYPE_T;
 }
    ps_seq->u1_pic_order_cnt_type = u4_temp;
    COPYTHECONTEXT("SPS: pic_order_cnt_type",ps_seq->u1_pic_order_cnt_type);

    ps_seq->u1_num_ref_frames_in_pic_order_cnt_cycle = 1;
 if(ps_seq->u1_pic_order_cnt_type == 0)
 {
        u4_temp = 4 + ih264d_uev(pu4_bitstrm_ofst, pu4_bitstrm_buf);
 if(u4_temp > MAX_BITS_IN_POC_LSB)
 {
 return ERROR_INV_SPS_PPS_T;
 }
        ps_seq->u1_log2_max_pic_order_cnt_lsb_minus = u4_temp;
        ps_seq->i4_max_pic_order_cntLsb = (1 << u4_temp);
        COPYTHECONTEXT("SPS: log2_max_pic_order_cnt_lsb_minus4",(u4_temp - 4));
 }
 else if(ps_seq->u1_pic_order_cnt_type == 1)
 {
        ps_seq->u1_delta_pic_order_always_zero_flag = ih264d_get_bit_h264(
                        ps_bitstrm);
        COPYTHECONTEXT("SPS: delta_pic_order_always_zero_flag",
                        ps_seq->u1_delta_pic_order_always_zero_flag);

        ps_seq->i4_ofst_for_non_ref_pic = ih264d_sev(pu4_bitstrm_ofst,
                                                     pu4_bitstrm_buf);
        COPYTHECONTEXT("SPS: offset_for_non_ref_pic",
                        ps_seq->i4_ofst_for_non_ref_pic);

        ps_seq->i4_ofst_for_top_to_bottom_field = ih264d_sev(
                        pu4_bitstrm_ofst, pu4_bitstrm_buf);
        COPYTHECONTEXT("SPS: offset_for_top_to_bottom_field",
                        ps_seq->i4_ofst_for_top_to_bottom_field);

        u4_temp = ih264d_uev(pu4_bitstrm_ofst, pu4_bitstrm_buf);
 if(u4_temp > 255)
 return ERROR_INV_SPS_PPS_T;
        ps_seq->u1_num_ref_frames_in_pic_order_cnt_cycle = u4_temp;
        COPYTHECONTEXT("SPS: num_ref_frames_in_pic_order_cnt_cycle",
                        ps_seq->u1_num_ref_frames_in_pic_order_cnt_cycle);

 for(i = 0; i < ps_seq->u1_num_ref_frames_in_pic_order_cnt_cycle; i++)
 {
            ps_seq->i4_ofst_for_ref_frame[i] = ih264d_sev(
                            pu4_bitstrm_ofst, pu4_bitstrm_buf);
            COPYTHECONTEXT("SPS: offset_for_ref_frame",
                            ps_seq->i4_ofst_for_ref_frame[i]);
 }
 }

    u4_temp = ih264d_uev(pu4_bitstrm_ofst, pu4_bitstrm_buf);

 if((u4_temp > H264_MAX_REF_PICS))
 {
 return ERROR_NUM_REF;
 }

 /* Compare with older num_ref_frames is header is already once */
 if((ps_dec->i4_header_decoded & 1) && (ps_seq->u1_num_ref_frames != u4_temp))
 {
        ps_dec->u1_res_changed = 1;
 return IVD_RES_CHANGED;
 }

    ps_seq->u1_num_ref_frames = u4_temp;
    COPYTHECONTEXT("SPS: num_ref_frames",ps_seq->u1_num_ref_frames);

    ps_seq->u1_gaps_in_frame_num_value_allowed_flag = ih264d_get_bit_h264(
                    ps_bitstrm);
    COPYTHECONTEXT("SPS: gaps_in_frame_num_value_allowed_flag",
                    ps_seq->u1_gaps_in_frame_num_value_allowed_flag);

 /*--------------------------------------------------------------------*/
 /* Decode FrameWidth and FrameHeight and related values               */
 /*--------------------------------------------------------------------*/
    ps_seq->u2_frm_wd_in_mbs = 1
 + ih264d_uev(pu4_bitstrm_ofst, pu4_bitstrm_buf);
    COPYTHECONTEXT("SPS: pic_width_in_mbs_minus1",
                    ps_seq->u2_frm_wd_in_mbs - 1);
    u2_pic_wd = (ps_seq->u2_frm_wd_in_mbs << 4);

    pic_height_in_map_units_minus1 = ih264d_uev(pu4_bitstrm_ofst,
                                                pu4_bitstrm_buf);
    ps_seq->u2_frm_ht_in_mbs = 1 + pic_height_in_map_units_minus1;

    u2_pic_ht = (ps_seq->u2_frm_ht_in_mbs << 4);

 /*--------------------------------------------------------------------*/
 /* Get the value of MaxMbAddress and Number of bits needed for it     */
 /*--------------------------------------------------------------------*/
    ps_seq->u2_max_mb_addr = (ps_seq->u2_frm_wd_in_mbs
 * ps_seq->u2_frm_ht_in_mbs) - 1;

    ps_seq->u2_total_num_of_mbs = ps_seq->u2_max_mb_addr + 1;

    ps_seq->u1_level_idc = ih264d_correct_level_idc(
                    u1_level_idc, ps_seq->u2_total_num_of_mbs);

    u1_frm = ih264d_get_bit_h264(ps_bitstrm);
 if((ps_dec->i4_header_decoded & 1) && (ps_seq->u1_frame_mbs_only_flag != u1_frm))
 {
        ps_dec->u1_res_changed = 1;
 return IVD_RES_CHANGED;
 }

    ps_seq->u1_frame_mbs_only_flag = u1_frm;


     COPYTHECONTEXT("SPS: frame_mbs_only_flag", u1_frm);
 
     if(!u1_frm)
        u1_mb_aff_flag = ih264d_get_bit_h264(ps_bitstrm);

    if((ps_dec->i4_header_decoded & 1)
                    && (ps_seq->u1_mb_aff_flag != u1_mb_aff_flag))
    {
        ps_dec->u1_res_changed = 1;
        return IVD_RES_CHANGED;
    }

    if(!u1_frm)
     {
         u2_pic_ht <<= 1;
        ps_seq->u1_mb_aff_flag = u1_mb_aff_flag;
         COPYTHECONTEXT("SPS: mb_adaptive_frame_field_flag",
                         ps_seq->u1_mb_aff_flag);
 
 }
 else
        ps_seq->u1_mb_aff_flag = 0;

    ps_seq->u1_direct_8x8_inference_flag = ih264d_get_bit_h264(ps_bitstrm);

    COPYTHECONTEXT("SPS: direct_8x8_inference_flag",
                    ps_seq->u1_direct_8x8_inference_flag);

 /* G050 */
    u1_frame_cropping_flag = ih264d_get_bit_h264(ps_bitstrm);
    COPYTHECONTEXT("SPS: frame_cropping_flag",u1_frame_cropping_flag);

 if(u1_frame_cropping_flag)
 {
        u1_frame_cropping_rect_left_ofst = ih264d_uev(pu4_bitstrm_ofst,
                                                      pu4_bitstrm_buf);
        COPYTHECONTEXT("SPS: frame_cropping_rect_left_offset",
                        u1_frame_cropping_rect_left_ofst);
        u1_frame_cropping_rect_right_ofst = ih264d_uev(pu4_bitstrm_ofst,
                                                       pu4_bitstrm_buf);
        COPYTHECONTEXT("SPS: frame_cropping_rect_right_offset",
                        u1_frame_cropping_rect_right_ofst);
        u1_frame_cropping_rect_top_ofst = ih264d_uev(pu4_bitstrm_ofst,
                                                     pu4_bitstrm_buf);
        COPYTHECONTEXT("SPS: frame_cropping_rect_top_offset",
                        u1_frame_cropping_rect_top_ofst);
        u1_frame_cropping_rect_bottom_ofst = ih264d_uev(pu4_bitstrm_ofst,
                                                        pu4_bitstrm_buf);
        COPYTHECONTEXT("SPS: frame_cropping_rect_bottom_offset",
                        u1_frame_cropping_rect_bottom_ofst);
 }
 /* G050 */

    ps_seq->u1_vui_parameters_present_flag = ih264d_get_bit_h264(ps_bitstrm);
    COPYTHECONTEXT("SPS: vui_parameters_present_flag",
                    ps_seq->u1_vui_parameters_present_flag);

    u2_frm_wd_y = u2_pic_wd + (UWORD8)(PAD_LEN_Y_H << 1);
 if(1 == ps_dec->u4_share_disp_buf)
 {
 if(ps_dec->u4_app_disp_width > u2_frm_wd_y)
            u2_frm_wd_y = ps_dec->u4_app_disp_width;
 }

    u2_frm_ht_y = u2_pic_ht + (UWORD8)(PAD_LEN_Y_V << 2);
    u2_frm_wd_uv = u2_pic_wd + (UWORD8)(PAD_LEN_UV_H << 2);
    u2_frm_wd_uv = MAX(u2_frm_wd_uv, u2_frm_wd_y);

    u2_frm_ht_uv = (u2_pic_ht >> 1) + (UWORD8)(PAD_LEN_UV_V << 2);
    u2_frm_ht_uv = MAX(u2_frm_ht_uv, (u2_frm_ht_y >> 1));


 /* Calculate display picture width, height and start u4_ofst from YUV420 */
 /* pictute buffers as per cropping information parsed above             */
 {
        UWORD16 u2_rgt_ofst = 0;
        UWORD16 u2_lft_ofst = 0;
        UWORD16 u2_top_ofst = 0;
        UWORD16 u2_btm_ofst = 0;
        UWORD8 u1_frm_mbs_flag;
        UWORD8 u1_vert_mult_factor;

 if(u1_frame_cropping_flag)
 {
 /* Calculate right and left u4_ofst for cropped picture           */
            u2_rgt_ofst = u1_frame_cropping_rect_right_ofst << 1;
            u2_lft_ofst = u1_frame_cropping_rect_left_ofst << 1;

 /* Know frame MBs only u4_flag                                      */
            u1_frm_mbs_flag = (1 == ps_seq->u1_frame_mbs_only_flag);

 /* Simplify the vertical u4_ofst calculation from field/frame     */
            u1_vert_mult_factor = (2 - u1_frm_mbs_flag);

 /* Calculate bottom and top u4_ofst for cropped  picture          */
            u2_btm_ofst = (u1_frame_cropping_rect_bottom_ofst
 << u1_vert_mult_factor);
            u2_top_ofst = (u1_frame_cropping_rect_top_ofst
 << u1_vert_mult_factor);
 }

 /* Calculate u4_ofst from start of YUV 420 picture buffer to start of*/
 /* cropped picture buffer                                           */
        u2_crop_offset_y = (u2_frm_wd_y * u2_top_ofst) + (u2_lft_ofst);
        u2_crop_offset_uv = (u2_frm_wd_uv * (u2_top_ofst >> 1))
 + (u2_lft_ofst >> 1) * YUV420SP_FACTOR;
 /* Calculate the display picture width and height based on crop      */
 /* information                                                       */
        i4_cropped_ht = u2_pic_ht - (u2_btm_ofst + u2_top_ofst);
        i4_cropped_wd = u2_pic_wd - (u2_rgt_ofst + u2_lft_ofst);

 if((i4_cropped_ht < MB_SIZE) || (i4_cropped_wd < MB_SIZE))
 {
 return ERROR_INV_SPS_PPS_T;
 }

 if((ps_dec->i4_header_decoded & 1) && (ps_dec->u2_pic_wd != u2_pic_wd))
 {
            ps_dec->u1_res_changed = 1;
 return IVD_RES_CHANGED;
 }
 if((ps_dec->i4_header_decoded & 1) && (ps_dec->u2_pic_ht != u2_pic_ht))
 {
            ps_dec->u1_res_changed = 1;
 return IVD_RES_CHANGED;
 }

 /* Check for unsupported resolutions */
 if((u2_pic_wd > H264_MAX_FRAME_WIDTH) || (u2_pic_ht > H264_MAX_FRAME_HEIGHT)
 || (u2_pic_wd < H264_MIN_FRAME_WIDTH) || (u2_pic_ht < H264_MIN_FRAME_HEIGHT)
 || (u2_pic_wd * (UWORD32)u2_pic_ht > H264_MAX_FRAME_SIZE))
 {
 return IVD_STREAM_WIDTH_HEIGHT_NOT_SUPPORTED;
 }

 /* If MBAff is enabled, decoder support is limited to streams with
         * width less than half of H264_MAX_FRAME_WIDTH.
         * In case of MBAff decoder processes two rows at a time
         */
 if((u2_pic_wd << ps_seq->u1_mb_aff_flag) > H264_MAX_FRAME_WIDTH)
 {
 return IVD_STREAM_WIDTH_HEIGHT_NOT_SUPPORTED;
 }

 }

 /* Backup u4_num_reorder_frames if header is already decoded */
 if((ps_dec->i4_header_decoded & 1) &&
 (1 == ps_seq->u1_vui_parameters_present_flag) &&
 (1 == ps_seq->s_vui.u1_bitstream_restriction_flag))
 {
        u4_num_reorder_frames =  ps_seq->s_vui.u4_num_reorder_frames;
 }
 else
 {
        u4_num_reorder_frames = -1;
 }
 if(1 == ps_seq->u1_vui_parameters_present_flag)
 {
        ret = ih264d_parse_vui_parametres(&ps_seq->s_vui, ps_bitstrm);
 if(ret != OK)
 return ret;
 }

 /* Compare older u4_num_reorder_frames with the new one if header is already decoded */
 if((ps_dec->i4_header_decoded & 1) &&
 (-1 != (WORD32)u4_num_reorder_frames) &&
 (1 == ps_seq->u1_vui_parameters_present_flag) &&
 (1 == ps_seq->s_vui.u1_bitstream_restriction_flag) &&
 (ps_seq->s_vui.u4_num_reorder_frames != u4_num_reorder_frames))
 {
        ps_dec->u1_res_changed = 1;
 return IVD_RES_CHANGED;
 }

 /* In case bitstream read has exceeded the filled size, then
     return an error */
 if (ps_bitstrm->u4_ofst > ps_bitstrm->u4_max_ofst)
 {
 return ERROR_INV_SPS_PPS_T;
 }

 /*--------------------------------------------------------------------*/
 /* All initializations to ps_dec are beyond this point                */
 /*--------------------------------------------------------------------*/
    ps_dec->u2_disp_height = i4_cropped_ht;
    ps_dec->u2_disp_width = i4_cropped_wd;

    ps_dec->u2_pic_wd = u2_pic_wd;
    ps_dec->u2_pic_ht = u2_pic_ht;

 /* Determining the Width and Height of Frame from that of Picture */
    ps_dec->u2_frm_wd_y = u2_frm_wd_y;
    ps_dec->u2_frm_ht_y = u2_frm_ht_y;

    ps_dec->u2_frm_wd_uv = u2_frm_wd_uv;
    ps_dec->u2_frm_ht_uv = u2_frm_ht_uv;
    ps_dec->s_pad_mgr.u1_pad_len_y_v = (UWORD8)(PAD_LEN_Y_V << (1 - u1_frm));
    ps_dec->s_pad_mgr.u1_pad_len_cr_v = (UWORD8)(PAD_LEN_UV_V << (1 - u1_frm));

    ps_dec->u2_frm_wd_in_mbs = ps_seq->u2_frm_wd_in_mbs;
    ps_dec->u2_frm_ht_in_mbs = ps_seq->u2_frm_ht_in_mbs;

    ps_dec->u2_crop_offset_y = u2_crop_offset_y;
    ps_dec->u2_crop_offset_uv = u2_crop_offset_uv;

    ps_seq->u1_is_valid = TRUE;
    ps_dec->ps_sps[u1_seq_parameter_set_id] = *ps_seq;
    ps_dec->ps_cur_sps = &ps_dec->ps_sps[u1_seq_parameter_set_id];

 return OK;
}
