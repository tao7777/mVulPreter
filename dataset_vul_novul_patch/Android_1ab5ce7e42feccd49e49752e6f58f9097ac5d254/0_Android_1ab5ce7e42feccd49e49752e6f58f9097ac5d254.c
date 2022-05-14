IHEVCD_ERROR_T ihevcd_parse_sps(codec_t *ps_codec)
{
    IHEVCD_ERROR_T ret = (IHEVCD_ERROR_T)IHEVCD_SUCCESS;
    WORD32 value;

    WORD32 i;
    WORD32 vps_id;
    WORD32 sps_max_sub_layers;
    WORD32 sps_id;
    WORD32 sps_temporal_id_nesting_flag;
 sps_t *ps_sps;
 profile_tier_lvl_info_t s_ptl;
 bitstrm_t *ps_bitstrm = &ps_codec->s_parse.s_bitstrm;


    BITS_PARSE("video_parameter_set_id", value, ps_bitstrm, 4);
    vps_id = value;
    vps_id = CLIP3(vps_id, 0, MAX_VPS_CNT - 1);

    BITS_PARSE("sps_max_sub_layers_minus1", value, ps_bitstrm, 3);
    sps_max_sub_layers = value + 1;
    sps_max_sub_layers = CLIP3(sps_max_sub_layers, 1, 7);

    BITS_PARSE("sps_temporal_id_nesting_flag", value, ps_bitstrm, 1);
    sps_temporal_id_nesting_flag = value;

    ret = ihevcd_profile_tier_level(ps_bitstrm, &(s_ptl), 1,
 (sps_max_sub_layers - 1));

    UEV_PARSE("seq_parameter_set_id", value, ps_bitstrm);
    sps_id = value;

 if((sps_id >= MAX_SPS_CNT) || (sps_id < 0))
 {
 if(ps_codec->i4_sps_done)
 return IHEVCD_UNSUPPORTED_SPS_ID;
 else
            sps_id = 0;
 }


    ps_sps = (ps_codec->s_parse.ps_sps_base + MAX_SPS_CNT - 1);
    ps_sps->i1_sps_id = sps_id;
    ps_sps->i1_vps_id = vps_id;
    ps_sps->i1_sps_max_sub_layers = sps_max_sub_layers;
    ps_sps->i1_sps_temporal_id_nesting_flag = sps_temporal_id_nesting_flag;
 /* This is used only during initialization to get reorder count etc */
    ps_codec->i4_sps_id = sps_id;
    memcpy(&ps_sps->s_ptl, &s_ptl, sizeof(profile_tier_lvl_info_t));

    UEV_PARSE("chroma_format_idc", value, ps_bitstrm);
    ps_sps->i1_chroma_format_idc = value;

 if(ps_sps->i1_chroma_format_idc != CHROMA_FMT_IDC_YUV420)
 {
        ps_codec->s_parse.i4_error_code = IHEVCD_UNSUPPORTED_CHROMA_FMT_IDC;
 return (IHEVCD_ERROR_T)IHEVCD_UNSUPPORTED_CHROMA_FMT_IDC;
 }

 if(CHROMA_FMT_IDC_YUV444_PLANES == ps_sps->i1_chroma_format_idc)
 {
        BITS_PARSE("separate_colour_plane_flag", value, ps_bitstrm, 1);
        ps_sps->i1_separate_colour_plane_flag = value;
 }
 else
 {
        ps_sps->i1_separate_colour_plane_flag = 0;
 }

    UEV_PARSE("pic_width_in_luma_samples", value, ps_bitstrm);
    ps_sps->i2_pic_width_in_luma_samples = value;

    UEV_PARSE("pic_height_in_luma_samples", value, ps_bitstrm);
    ps_sps->i2_pic_height_in_luma_samples = value;

 if((0 >= ps_sps->i2_pic_width_in_luma_samples) || (0 >= ps_sps->i2_pic_height_in_luma_samples))
 return IHEVCD_INVALID_PARAMETER;

 /* i2_pic_width_in_luma_samples and i2_pic_height_in_luma_samples
       should be multiples of min_cb_size. Here these are aligned to 8,
       i.e. smallest CB size */
    ps_sps->i2_pic_width_in_luma_samples = ALIGN8(ps_sps->i2_pic_width_in_luma_samples);
    ps_sps->i2_pic_height_in_luma_samples = ALIGN8(ps_sps->i2_pic_height_in_luma_samples);

 if((ps_sps->i2_pic_width_in_luma_samples > ps_codec->i4_max_wd) ||
 (ps_sps->i2_pic_width_in_luma_samples * ps_sps->i2_pic_height_in_luma_samples >
                       ps_codec->i4_max_wd * ps_codec->i4_max_ht) ||
 (ps_sps->i2_pic_height_in_luma_samples > MAX(ps_codec->i4_max_wd, ps_codec->i4_max_ht)))
 {
        ps_codec->i4_new_max_wd = ps_sps->i2_pic_width_in_luma_samples;
        ps_codec->i4_new_max_ht = ps_sps->i2_pic_height_in_luma_samples;
 return (IHEVCD_ERROR_T)IHEVCD_UNSUPPORTED_DIMENSIONS;
 }

    BITS_PARSE("pic_cropping_flag", value, ps_bitstrm, 1);
    ps_sps->i1_pic_cropping_flag = value;

 if(ps_sps->i1_pic_cropping_flag)
 {

        UEV_PARSE("pic_crop_left_offset", value, ps_bitstrm);
        ps_sps->i2_pic_crop_left_offset = value;

        UEV_PARSE("pic_crop_right_offset", value, ps_bitstrm);
        ps_sps->i2_pic_crop_right_offset = value;

        UEV_PARSE("pic_crop_top_offset", value, ps_bitstrm);
        ps_sps->i2_pic_crop_top_offset = value;

        UEV_PARSE("pic_crop_bottom_offset", value, ps_bitstrm);
        ps_sps->i2_pic_crop_bottom_offset = value;
 }
 else
 {
        ps_sps->i2_pic_crop_left_offset = 0;
        ps_sps->i2_pic_crop_right_offset = 0;
        ps_sps->i2_pic_crop_top_offset = 0;
        ps_sps->i2_pic_crop_bottom_offset = 0;
 }


    UEV_PARSE("bit_depth_luma_minus8", value, ps_bitstrm);
 if(0 != value)
 return IHEVCD_UNSUPPORTED_BIT_DEPTH;

    UEV_PARSE("bit_depth_chroma_minus8", value, ps_bitstrm);
 if(0 != value)
 return IHEVCD_UNSUPPORTED_BIT_DEPTH;

    UEV_PARSE("log2_max_pic_order_cnt_lsb_minus4", value, ps_bitstrm);
    ps_sps->i1_log2_max_pic_order_cnt_lsb = value + 4;

    BITS_PARSE("sps_sub_layer_ordering_info_present_flag", value, ps_bitstrm, 1);
    ps_sps->i1_sps_sub_layer_ordering_info_present_flag = value;


    i = (ps_sps->i1_sps_sub_layer_ordering_info_present_flag ? 0 : (ps_sps->i1_sps_max_sub_layers - 1));
 for(; i < ps_sps->i1_sps_max_sub_layers; i++)
 {

         UEV_PARSE("max_dec_pic_buffering", value, ps_bitstrm);
         ps_sps->ai1_sps_max_dec_pic_buffering[i] = value + 1;
 
        if(ps_sps->ai1_sps_max_dec_pic_buffering[i] > MAX_DPB_SIZE)
        {
            return IHEVCD_INVALID_PARAMETER;
        }

         UEV_PARSE("num_reorder_pics", value, ps_bitstrm);
         ps_sps->ai1_sps_max_num_reorder_pics[i] = value;
 
        if(ps_sps->ai1_sps_max_num_reorder_pics[i] > ps_sps->ai1_sps_max_dec_pic_buffering[i])
        {
            return IHEVCD_INVALID_PARAMETER;
        }

         UEV_PARSE("max_latency_increase", value, ps_bitstrm);
         ps_sps->ai1_sps_max_latency_increase[i] = value;
     }
    UEV_PARSE("log2_min_coding_block_size_minus3", value, ps_bitstrm);
    ps_sps->i1_log2_min_coding_block_size = value + 3;

    UEV_PARSE("log2_diff_max_min_coding_block_size", value, ps_bitstrm);
    ps_sps->i1_log2_diff_max_min_coding_block_size = value;

    UEV_PARSE("log2_min_transform_block_size_minus2", value, ps_bitstrm);
    ps_sps->i1_log2_min_transform_block_size = value + 2;

    UEV_PARSE("log2_diff_max_min_transform_block_size", value, ps_bitstrm);
    ps_sps->i1_log2_diff_max_min_transform_block_size = value;

    ps_sps->i1_log2_max_transform_block_size = ps_sps->i1_log2_min_transform_block_size +
                    ps_sps->i1_log2_diff_max_min_transform_block_size;

    ps_sps->i1_log2_ctb_size = ps_sps->i1_log2_min_coding_block_size +
                    ps_sps->i1_log2_diff_max_min_coding_block_size;

 if((ps_sps->i1_log2_min_coding_block_size < 3) ||
 (ps_sps->i1_log2_min_transform_block_size < 2) ||
 (ps_sps->i1_log2_diff_max_min_transform_block_size < 0) ||
 (ps_sps->i1_log2_max_transform_block_size > ps_sps->i1_log2_ctb_size) ||
 (ps_sps->i1_log2_ctb_size < 4) ||
 (ps_sps->i1_log2_ctb_size > 6))
 {
 return IHEVCD_INVALID_PARAMETER;
 }

    ps_sps->i1_log2_min_pcm_coding_block_size = 0;
    ps_sps->i1_log2_diff_max_min_pcm_coding_block_size = 0;

    UEV_PARSE("max_transform_hierarchy_depth_inter", value, ps_bitstrm);
    ps_sps->i1_max_transform_hierarchy_depth_inter = value;

    UEV_PARSE("max_transform_hierarchy_depth_intra", value, ps_bitstrm);
    ps_sps->i1_max_transform_hierarchy_depth_intra = value;

 /* String has a d (enabled) in order to match with HM */
    BITS_PARSE("scaling_list_enabled_flag", value, ps_bitstrm, 1);
    ps_sps->i1_scaling_list_enable_flag = value;

 if(ps_sps->i1_scaling_list_enable_flag)
 {
        COPY_DEFAULT_SCALING_LIST(ps_sps->pi2_scaling_mat);
        BITS_PARSE("sps_scaling_list_data_present_flag", value, ps_bitstrm, 1);
        ps_sps->i1_sps_scaling_list_data_present_flag = value;

 if(ps_sps->i1_sps_scaling_list_data_present_flag)
            ihevcd_scaling_list_data(ps_codec, ps_sps->pi2_scaling_mat);
 }
 else
 {
        COPY_FLAT_SCALING_LIST(ps_sps->pi2_scaling_mat);
 }
 /* String is asymmetric_motion_partitions_enabled_flag instead of amp_enabled_flag in order to match with HM */
    BITS_PARSE("asymmetric_motion_partitions_enabled_flag", value, ps_bitstrm, 1);
    ps_sps->i1_amp_enabled_flag = value;

    BITS_PARSE("sample_adaptive_offset_enabled_flag", value, ps_bitstrm, 1);
    ps_sps->i1_sample_adaptive_offset_enabled_flag = value;

    BITS_PARSE("pcm_enabled_flag", value, ps_bitstrm, 1);
    ps_sps->i1_pcm_enabled_flag = value;

 if(ps_sps->i1_pcm_enabled_flag)
 {
        BITS_PARSE("pcm_sample_bit_depth_luma", value, ps_bitstrm, 4);
        ps_sps->i1_pcm_sample_bit_depth_luma = value + 1;

        BITS_PARSE("pcm_sample_bit_depth_chroma", value, ps_bitstrm, 4);
        ps_sps->i1_pcm_sample_bit_depth_chroma = value + 1;

        UEV_PARSE("log2_min_pcm_coding_block_size_minus3", value, ps_bitstrm);
        ps_sps->i1_log2_min_pcm_coding_block_size = value + 3;

        UEV_PARSE("log2_diff_max_min_pcm_coding_block_size", value, ps_bitstrm);
        ps_sps->i1_log2_diff_max_min_pcm_coding_block_size = value;
        BITS_PARSE("pcm_loop_filter_disable_flag", value, ps_bitstrm, 1);
        ps_sps->i1_pcm_loop_filter_disable_flag = value;

 }
    UEV_PARSE("num_short_term_ref_pic_sets", value, ps_bitstrm);
    ps_sps->i1_num_short_term_ref_pic_sets = value;

    ps_sps->i1_num_short_term_ref_pic_sets = CLIP3(ps_sps->i1_num_short_term_ref_pic_sets, 0, MAX_STREF_PICS_SPS);

 for(i = 0; i < ps_sps->i1_num_short_term_ref_pic_sets; i++)
        ihevcd_short_term_ref_pic_set(ps_bitstrm, &ps_sps->as_stref_picset[0], ps_sps->i1_num_short_term_ref_pic_sets, i, &ps_sps->as_stref_picset[i]);

    BITS_PARSE("long_term_ref_pics_present_flag", value, ps_bitstrm, 1);
    ps_sps->i1_long_term_ref_pics_present_flag = value;

 if(ps_sps->i1_long_term_ref_pics_present_flag)
 {
        UEV_PARSE("num_long_term_ref_pics_sps", value, ps_bitstrm);
        ps_sps->i1_num_long_term_ref_pics_sps = value;

 for(i = 0; i < ps_sps->i1_num_long_term_ref_pics_sps; i++)
 {
            BITS_PARSE("lt_ref_pic_poc_lsb_sps[ i ]", value, ps_bitstrm, ps_sps->i1_log2_max_pic_order_cnt_lsb);
            ps_sps->ai1_lt_ref_pic_poc_lsb_sps[i] = value;

            BITS_PARSE("used_by_curr_pic_lt_sps_flag[ i ]", value, ps_bitstrm, 1);
            ps_sps->ai1_used_by_curr_pic_lt_sps_flag[i] = value;
 }
 }

    BITS_PARSE("sps_temporal_mvp_enable_flag", value, ps_bitstrm, 1);
    ps_sps->i1_sps_temporal_mvp_enable_flag = value;

 /* Print matches HM 8-2 */
    BITS_PARSE("sps_strong_intra_smoothing_enable_flag", value, ps_bitstrm, 1);
    ps_sps->i1_strong_intra_smoothing_enable_flag = value;

    BITS_PARSE("vui_parameters_present_flag", value, ps_bitstrm, 1);
    ps_sps->i1_vui_parameters_present_flag = value;

 if(ps_sps->i1_vui_parameters_present_flag)
        ihevcd_parse_vui_parameters(ps_bitstrm,
 &ps_sps->s_vui_parameters,
                                    ps_sps->i1_sps_max_sub_layers - 1);

    BITS_PARSE("sps_extension_flag", value, ps_bitstrm, 1);


 {
        WORD32 numerator;
        WORD32 ceil_offset;

        ceil_offset = (1 << ps_sps->i1_log2_ctb_size) - 1;
        numerator = ps_sps->i2_pic_width_in_luma_samples;

        ps_sps->i2_pic_wd_in_ctb = ((numerator + ceil_offset) /
 (1 << ps_sps->i1_log2_ctb_size));

        numerator = ps_sps->i2_pic_height_in_luma_samples;
        ps_sps->i2_pic_ht_in_ctb = ((numerator + ceil_offset) /
 (1 << ps_sps->i1_log2_ctb_size));

        ps_sps->i4_pic_size_in_ctb = ps_sps->i2_pic_ht_in_ctb *
                        ps_sps->i2_pic_wd_in_ctb;

 if(0 == ps_codec->i4_sps_done)
            ps_codec->s_parse.i4_next_ctb_indx = ps_sps->i4_pic_size_in_ctb;

        numerator = ps_sps->i2_pic_width_in_luma_samples;
        ps_sps->i2_pic_wd_in_min_cb = numerator  /
 (1 << ps_sps->i1_log2_min_coding_block_size);

        numerator = ps_sps->i2_pic_height_in_luma_samples;
        ps_sps->i2_pic_ht_in_min_cb = numerator  /
 (1 << ps_sps->i1_log2_min_coding_block_size);
 }
 if((0 != ps_codec->i4_first_pic_done) &&
 ((ps_codec->i4_wd != ps_sps->i2_pic_width_in_luma_samples) ||
 (ps_codec->i4_ht != ps_sps->i2_pic_height_in_luma_samples)))
 {
        ps_codec->i4_reset_flag = 1;
        ps_codec->i4_error_code = IVD_RES_CHANGED;
 return (IHEVCD_ERROR_T)IHEVCD_FAIL;
 }

 /* Update display width and display height */
 {
        WORD32 disp_wd, disp_ht;
        WORD32 crop_unit_x, crop_unit_y;
        crop_unit_x = 1;
        crop_unit_y = 1;

 if(CHROMA_FMT_IDC_YUV420 == ps_sps->i1_chroma_format_idc)
 {
            crop_unit_x = 2;
            crop_unit_y = 2;
 }

        disp_wd = ps_sps->i2_pic_width_in_luma_samples;
        disp_wd -= ps_sps->i2_pic_crop_left_offset * crop_unit_x;
        disp_wd -= ps_sps->i2_pic_crop_right_offset * crop_unit_x;


        disp_ht = ps_sps->i2_pic_height_in_luma_samples;
        disp_ht -= ps_sps->i2_pic_crop_top_offset * crop_unit_y;
        disp_ht -= ps_sps->i2_pic_crop_bottom_offset * crop_unit_y;

 if((0 >= disp_wd) || (0 >= disp_ht))
 return IHEVCD_INVALID_PARAMETER;

        ps_codec->i4_disp_wd = disp_wd;
        ps_codec->i4_disp_ht = disp_ht;


        ps_codec->i4_wd = ps_sps->i2_pic_width_in_luma_samples;
        ps_codec->i4_ht = ps_sps->i2_pic_height_in_luma_samples;

 {
            WORD32 ref_strd;
            ref_strd = ALIGN32(ps_sps->i2_pic_width_in_luma_samples + PAD_WD);
 if(ps_codec->i4_strd < ref_strd)
 {
                ps_codec->i4_strd = ref_strd;
 }
 }

 if(0 == ps_codec->i4_share_disp_buf)
 {
 if(ps_codec->i4_disp_strd < ps_codec->i4_disp_wd)
 {
                ps_codec->i4_disp_strd = ps_codec->i4_disp_wd;
 }
 }
 else
 {
 if(ps_codec->i4_disp_strd < ps_codec->i4_strd)
 {
                ps_codec->i4_disp_strd = ps_codec->i4_strd;
 }
 }
 }

    ps_codec->i4_sps_done = 1;
 return ret;
}
