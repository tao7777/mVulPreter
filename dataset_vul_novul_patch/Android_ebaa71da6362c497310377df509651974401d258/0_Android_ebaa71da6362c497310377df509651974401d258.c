IHEVCD_ERROR_T ihevcd_parse_pps(codec_t *ps_codec)
{
    IHEVCD_ERROR_T ret = (IHEVCD_ERROR_T)IHEVCD_SUCCESS;
    WORD32 value;
    WORD32 pps_id;

 pps_t *ps_pps;
 sps_t *ps_sps;
 bitstrm_t *ps_bitstrm = &ps_codec->s_parse.s_bitstrm;


 if(0 == ps_codec->i4_sps_done)
 return IHEVCD_INVALID_HEADER;

    UEV_PARSE("pic_parameter_set_id", value, ps_bitstrm);

    pps_id = value;
 if((pps_id >= MAX_PPS_CNT) || (pps_id < 0))
 {
 if(ps_codec->i4_pps_done)
 return IHEVCD_UNSUPPORTED_PPS_ID;
 else
            pps_id = 0;
 }


    ps_pps = (ps_codec->s_parse.ps_pps_base + MAX_PPS_CNT - 1);

    ps_pps->i1_pps_id = pps_id;

    UEV_PARSE("seq_parameter_set_id", value, ps_bitstrm);
    ps_pps->i1_sps_id = value;
    ps_pps->i1_sps_id = CLIP3(ps_pps->i1_sps_id, 0, MAX_SPS_CNT - 2);

    ps_sps = (ps_codec->s_parse.ps_sps_base + ps_pps->i1_sps_id);

 /* If the SPS that is being referred to has not been parsed,
     * copy an existing SPS to the current location */
 if(0 == ps_sps->i1_sps_valid)
 {
 return IHEVCD_INVALID_HEADER;

/*
        sps_t *ps_sps_ref = ps_codec->ps_sps_base;
        while(0 == ps_sps_ref->i1_sps_valid)
            ps_sps_ref++;
        ihevcd_copy_sps(ps_codec, ps_pps->i1_sps_id, ps_sps_ref->i1_sps_id);
*/
 }

    BITS_PARSE("dependent_slices_enabled_flag", value, ps_bitstrm, 1);
    ps_pps->i1_dependent_slice_enabled_flag = value;

    BITS_PARSE("output_flag_present_flag", value, ps_bitstrm, 1);
    ps_pps->i1_output_flag_present_flag = value;

    BITS_PARSE("num_extra_slice_header_bits", value, ps_bitstrm, 3);
    ps_pps->i1_num_extra_slice_header_bits = value;


    BITS_PARSE("sign_data_hiding_flag", value, ps_bitstrm, 1);
    ps_pps->i1_sign_data_hiding_flag = value;

    BITS_PARSE("cabac_init_present_flag", value, ps_bitstrm, 1);
    ps_pps->i1_cabac_init_present_flag = value;

    UEV_PARSE("num_ref_idx_l0_default_active_minus1", value, ps_bitstrm);
    ps_pps->i1_num_ref_idx_l0_default_active = value + 1;

    UEV_PARSE("num_ref_idx_l1_default_active_minus1", value, ps_bitstrm);
    ps_pps->i1_num_ref_idx_l1_default_active = value + 1;

    SEV_PARSE("pic_init_qp_minus26", value, ps_bitstrm);
    ps_pps->i1_pic_init_qp = value + 26;

    BITS_PARSE("constrained_intra_pred_flag", value, ps_bitstrm, 1);
    ps_pps->i1_constrained_intra_pred_flag = value;

    BITS_PARSE("transform_skip_enabled_flag", value, ps_bitstrm, 1);
    ps_pps->i1_transform_skip_enabled_flag = value;

    BITS_PARSE("cu_qp_delta_enabled_flag", value, ps_bitstrm, 1);
    ps_pps->i1_cu_qp_delta_enabled_flag = value;

 if(ps_pps->i1_cu_qp_delta_enabled_flag)
 {
        UEV_PARSE("diff_cu_qp_delta_depth", value, ps_bitstrm);
        ps_pps->i1_diff_cu_qp_delta_depth = value;
 }
 else
 {
        ps_pps->i1_diff_cu_qp_delta_depth = 0;
 }
    ps_pps->i1_log2_min_cu_qp_delta_size = ps_sps->i1_log2_ctb_size - ps_pps->i1_diff_cu_qp_delta_depth;
 /* Print different */
    SEV_PARSE("cb_qp_offset", value, ps_bitstrm);
    ps_pps->i1_pic_cb_qp_offset = value;

 /* Print different */
    SEV_PARSE("cr_qp_offset", value, ps_bitstrm);
    ps_pps->i1_pic_cr_qp_offset = value;

 /* Print different */
    BITS_PARSE("slicelevel_chroma_qp_flag", value, ps_bitstrm, 1);
    ps_pps->i1_pic_slice_level_chroma_qp_offsets_present_flag = value;

    BITS_PARSE("weighted_pred_flag", value, ps_bitstrm, 1);
    ps_pps->i1_weighted_pred_flag = value;

    BITS_PARSE("weighted_bipred_flag", value, ps_bitstrm, 1);
    ps_pps->i1_weighted_bipred_flag = value;

    BITS_PARSE("transquant_bypass_enable_flag", value, ps_bitstrm, 1);
    ps_pps->i1_transquant_bypass_enable_flag = value;

    BITS_PARSE("tiles_enabled_flag", value, ps_bitstrm, 1);
    ps_pps->i1_tiles_enabled_flag = value;

    BITS_PARSE("entropy_coding_sync_enabled_flag", value, ps_bitstrm, 1);
    ps_pps->i1_entropy_coding_sync_enabled_flag = value;


     ps_pps->i1_loop_filter_across_tiles_enabled_flag = 0;
     if(ps_pps->i1_tiles_enabled_flag)
     {
        WORD32 wd = ALIGN64(ps_codec->i4_wd);
        WORD32 ht = ALIGN64(ps_codec->i4_ht);

        WORD32 max_tile_cols = (wd + MIN_TILE_WD - 1) / MIN_TILE_WD;
        WORD32 max_tile_rows = (ht + MIN_TILE_HT - 1) / MIN_TILE_HT;

         UEV_PARSE("num_tile_columns_minus1", value, ps_bitstrm);
         ps_pps->i1_num_tile_columns = value + 1;
 
        UEV_PARSE("num_tile_rows_minus1", value, ps_bitstrm);

         ps_pps->i1_num_tile_rows = value + 1;
 
         if((ps_pps->i1_num_tile_columns < 1) ||
                        (ps_pps->i1_num_tile_columns > max_tile_cols) ||
                         (ps_pps->i1_num_tile_rows < 1) ||
                        (ps_pps->i1_num_tile_rows > max_tile_rows))
             return IHEVCD_INVALID_HEADER;
 
         BITS_PARSE("uniform_spacing_flag", value, ps_bitstrm, 1);
        ps_pps->i1_uniform_spacing_flag = value;


 {

            WORD32 start;
            WORD32 i, j;


            start = 0;
 for(i = 0; i < ps_pps->i1_num_tile_columns; i++)
 {
 tile_t *ps_tile;
 if(!ps_pps->i1_uniform_spacing_flag)
 {
 if(i < (ps_pps->i1_num_tile_columns - 1))
 {
                        UEV_PARSE("column_width_minus1[ i ]", value, ps_bitstrm);
                        value += 1;
 }
 else
 {
                        value = ps_sps->i2_pic_wd_in_ctb - start;
 }
 }
 else
 {
                    value = ((i + 1) * ps_sps->i2_pic_wd_in_ctb) / ps_pps->i1_num_tile_columns -
 (i * ps_sps->i2_pic_wd_in_ctb) / ps_pps->i1_num_tile_columns;
 }

 for(j = 0; j < ps_pps->i1_num_tile_rows; j++)
 {
                    ps_tile = ps_pps->ps_tile + j * ps_pps->i1_num_tile_columns + i;
                    ps_tile->u1_pos_x = start;
                    ps_tile->u2_wd = value;
 }
                start += value;

 if((start > ps_sps->i2_pic_wd_in_ctb) ||
 (value <= 0))
 return IHEVCD_INVALID_HEADER;
 }

            start = 0;
 for(i = 0; i < (ps_pps->i1_num_tile_rows); i++)
 {
 tile_t *ps_tile;
 if(!ps_pps->i1_uniform_spacing_flag)
 {
 if(i < (ps_pps->i1_num_tile_rows - 1))
 {

                        UEV_PARSE("row_height_minus1[ i ]", value, ps_bitstrm);
                        value += 1;
 }
 else
 {
                        value = ps_sps->i2_pic_ht_in_ctb - start;
 }
 }
 else
 {
                    value = ((i + 1) * ps_sps->i2_pic_ht_in_ctb) / ps_pps->i1_num_tile_rows -
 (i * ps_sps->i2_pic_ht_in_ctb) / ps_pps->i1_num_tile_rows;
 }

 for(j = 0; j < ps_pps->i1_num_tile_columns; j++)
 {
                    ps_tile = ps_pps->ps_tile + i * ps_pps->i1_num_tile_columns + j;
                    ps_tile->u1_pos_y = start;
                    ps_tile->u2_ht = value;
 }
                start += value;

 if((start > ps_sps->i2_pic_ht_in_ctb) ||
 (value <= 0))
 return IHEVCD_INVALID_HEADER;
 }
 }


        BITS_PARSE("loop_filter_across_tiles_enabled_flag", value, ps_bitstrm, 1);
        ps_pps->i1_loop_filter_across_tiles_enabled_flag = value;

 }
 else
 {
 /* If tiles are not present, set first tile in each PPS to have tile
        width and height equal to picture width and height */
        ps_pps->i1_num_tile_columns = 1;
        ps_pps->i1_num_tile_rows = 1;
        ps_pps->i1_uniform_spacing_flag = 1;

        ps_pps->ps_tile->u1_pos_x = 0;
        ps_pps->ps_tile->u1_pos_y = 0;
        ps_pps->ps_tile->u2_wd = ps_sps->i2_pic_wd_in_ctb;
        ps_pps->ps_tile->u2_ht = ps_sps->i2_pic_ht_in_ctb;
 }

    BITS_PARSE("loop_filter_across_slices_enabled_flag", value, ps_bitstrm, 1);
    ps_pps->i1_loop_filter_across_slices_enabled_flag = value;

    BITS_PARSE("deblocking_filter_control_present_flag", value, ps_bitstrm, 1);
    ps_pps->i1_deblocking_filter_control_present_flag = value;

 /* Default values */
    ps_pps->i1_pic_disable_deblocking_filter_flag = 0;
    ps_pps->i1_deblocking_filter_override_enabled_flag = 0;
    ps_pps->i1_beta_offset_div2 = 0;
    ps_pps->i1_tc_offset_div2 = 0;

 if(ps_pps->i1_deblocking_filter_control_present_flag)
 {

        BITS_PARSE("deblocking_filter_override_enabled_flag", value, ps_bitstrm, 1);
        ps_pps->i1_deblocking_filter_override_enabled_flag = value;

        BITS_PARSE("pic_disable_deblocking_filter_flag", value, ps_bitstrm, 1);
        ps_pps->i1_pic_disable_deblocking_filter_flag = value;

 if(!ps_pps->i1_pic_disable_deblocking_filter_flag)
 {

            SEV_PARSE("pps_beta_offset_div2", value, ps_bitstrm);
            ps_pps->i1_beta_offset_div2 = value;

            SEV_PARSE("pps_tc_offset_div2", value, ps_bitstrm);
            ps_pps->i1_tc_offset_div2 = value;

 }
 }

    BITS_PARSE("pps_scaling_list_data_present_flag", value, ps_bitstrm, 1);
    ps_pps->i1_pps_scaling_list_data_present_flag = value;

 if(ps_pps->i1_pps_scaling_list_data_present_flag)
 {
        COPY_DEFAULT_SCALING_LIST(ps_pps->pi2_scaling_mat);
        ihevcd_scaling_list_data(ps_codec, ps_pps->pi2_scaling_mat);
 }

    BITS_PARSE("lists_modification_present_flag", value, ps_bitstrm, 1);
    ps_pps->i1_lists_modification_present_flag = value;
    UEV_PARSE("log2_parallel_merge_level_minus2", value, ps_bitstrm);
    ps_pps->i1_log2_parallel_merge_level = value + 2;

    BITS_PARSE("slice_header_extension_present_flag", value, ps_bitstrm, 1);
    ps_pps->i1_slice_header_extension_present_flag = value;
 /* Not present in HM */
    BITS_PARSE("pps_extension_flag", value, ps_bitstrm, 1);

    ps_codec->i4_pps_done = 1;
 return ret;
}
