WORD32 ihevcd_parse_transform_tree(codec_t *ps_codec,
                                   WORD32 x0, WORD32 y0,
                                   WORD32 cu_x_base, WORD32 cu_y_base,
                                   WORD32 log2_trafo_size,
                                   WORD32 trafo_depth,
                                   WORD32 blk_idx,
                                   WORD32 intra_pred_mode)
{
    IHEVCD_ERROR_T ret = (IHEVCD_ERROR_T)IHEVCD_SUCCESS;
 sps_t *ps_sps;
 pps_t *ps_pps;
    WORD32 value;
    WORD32 x1, y1;
    WORD32 max_trafo_depth;

 bitstrm_t *ps_bitstrm = &ps_codec->s_parse.s_bitstrm;
    WORD32 intra_split_flag;
    WORD32 split_transform_flag;
    WORD32 ctxt_idx;
 cab_ctxt_t *ps_cabac = &ps_codec->s_parse.s_cabac;

    max_trafo_depth = ps_codec->s_parse.s_cu.i4_max_trafo_depth;
    ps_sps = ps_codec->s_parse.ps_sps;
    ps_pps = ps_codec->s_parse.ps_pps;
    intra_split_flag = ps_codec->s_parse.s_cu.i4_intra_split_flag;

 {
        split_transform_flag = 0;
 if((log2_trafo_size <= ps_sps->i1_log2_max_transform_block_size) &&
 (log2_trafo_size > ps_sps->i1_log2_min_transform_block_size) &&
 (trafo_depth < max_trafo_depth) &&
 !(intra_split_flag && (trafo_depth == 0)))
 {
 /* encode the split transform flag, context derived as per Table9-37 */
            ctxt_idx = IHEVC_CAB_SPLIT_TFM + (5 - log2_trafo_size);

            TRACE_CABAC_CTXT("split_transform_flag", ps_cabac->u4_range, ctxt_idx);
            split_transform_flag = ihevcd_cabac_decode_bin(ps_cabac, ps_bitstrm, ctxt_idx);
            AEV_TRACE("split_transform_flag", split_transform_flag,
                      ps_cabac->u4_range);

 }
 else
 {
            WORD32 inter_split_flag = 0;

 if((0 == ps_sps->i1_max_transform_hierarchy_depth_inter) &&
 (PRED_MODE_INTER == ps_codec->s_parse.s_cu.i4_pred_mode) &&
 (PART_2Nx2N != ps_codec->s_parse.s_cu.i4_part_mode) &&
 (0 == trafo_depth))
 {
                inter_split_flag = 1;
 }

 if((log2_trafo_size > ps_sps->i1_log2_max_transform_block_size) ||
 ((1 == intra_split_flag) && (0 == trafo_depth)) ||
 (1 == inter_split_flag))
 {
                split_transform_flag = 1;
 }
 }

 if(0 == trafo_depth)
 {
            ps_codec->s_parse.s_cu.ai1_cbf_cr[trafo_depth] = 0;
            ps_codec->s_parse.s_cu.ai1_cbf_cb[trafo_depth] = 0;
 }
 else
 {
            ps_codec->s_parse.s_cu.ai1_cbf_cb[trafo_depth] = ps_codec->s_parse.s_cu.ai1_cbf_cb[trafo_depth - 1];
            ps_codec->s_parse.s_cu.ai1_cbf_cr[trafo_depth] = ps_codec->s_parse.s_cu.ai1_cbf_cr[trafo_depth - 1];
 }
 if(trafo_depth == 0 || log2_trafo_size > 2)
 {
            ctxt_idx = IHEVC_CAB_CBCR_IDX + trafo_depth;
 /* CBF for Cb/Cr is sent only if the parent CBF for Cb/Cr is non-zero */
 if((trafo_depth == 0) || ps_codec->s_parse.s_cu.ai1_cbf_cb[trafo_depth - 1])
 {
                TRACE_CABAC_CTXT("cbf_cb", ps_cabac->u4_range, ctxt_idx);
                value = ihevcd_cabac_decode_bin(ps_cabac, ps_bitstrm, ctxt_idx);
                AEV_TRACE("cbf_cb", value, ps_cabac->u4_range);
                ps_codec->s_parse.s_cu.ai1_cbf_cb[trafo_depth] = value;
 }

 if((trafo_depth == 0) || ps_codec->s_parse.s_cu.ai1_cbf_cr[trafo_depth - 1])
 {
                TRACE_CABAC_CTXT("cbf_cr", ps_cabac->u4_range, ctxt_idx);
                value = ihevcd_cabac_decode_bin(ps_cabac, ps_bitstrm, ctxt_idx);
                AEV_TRACE("cbf_cr", value, ps_cabac->u4_range);
                ps_codec->s_parse.s_cu.ai1_cbf_cr[trafo_depth] = value;
 }
 }
 if(split_transform_flag)
 {
            WORD32 intra_pred_mode_tmp;
            x1 = x0 + ((1 << log2_trafo_size) >> 1);
            y1 = y0 + ((1 << log2_trafo_size) >> 1);

 /* For transform depth of zero, intra pred mode as decoded at CU */
 /* level is sent to the transform tree nodes */
 /* When depth is non-zero intra pred mode of parent node is sent */
 /* This takes care of passing correct mode to all the child nodes */
            intra_pred_mode_tmp = trafo_depth ? intra_pred_mode : ps_codec->s_parse.s_cu.ai4_intra_luma_pred_mode[0];
            ihevcd_parse_transform_tree(ps_codec, x0, y0, x0, y0, log2_trafo_size - 1, trafo_depth + 1, 0, intra_pred_mode_tmp);

            intra_pred_mode_tmp = trafo_depth ? intra_pred_mode : ps_codec->s_parse.s_cu.ai4_intra_luma_pred_mode[1];
            ihevcd_parse_transform_tree(ps_codec, x1, y0, x0, y0, log2_trafo_size - 1, trafo_depth + 1, 1, intra_pred_mode_tmp);

            intra_pred_mode_tmp = trafo_depth ? intra_pred_mode : ps_codec->s_parse.s_cu.ai4_intra_luma_pred_mode[2];
            ihevcd_parse_transform_tree(ps_codec, x0, y1, x0, y0, log2_trafo_size - 1, trafo_depth + 1, 2, intra_pred_mode_tmp);

            intra_pred_mode_tmp = trafo_depth ? intra_pred_mode : ps_codec->s_parse.s_cu.ai4_intra_luma_pred_mode[3];
            ihevcd_parse_transform_tree(ps_codec, x1, y1, x0, y0, log2_trafo_size - 1, trafo_depth + 1, 3, intra_pred_mode_tmp);

 }
 else
 {
            WORD32 ctb_x_base;
            WORD32 ctb_y_base;
            WORD32 cu_qp_delta_abs;



 tu_t *ps_tu = ps_codec->s_parse.ps_tu;
            cu_qp_delta_abs = 0;
            ctb_x_base = ps_codec->s_parse.i4_ctb_x << ps_sps->i1_log2_ctb_size;
            ctb_y_base = ps_codec->s_parse.i4_ctb_y << ps_sps->i1_log2_ctb_size;

 if((ps_codec->s_parse.s_cu.i4_pred_mode == PRED_MODE_INTRA) ||
 (trafo_depth != 0) ||
 (ps_codec->s_parse.s_cu.ai1_cbf_cb[trafo_depth]) ||
 (ps_codec->s_parse.s_cu.ai1_cbf_cr[trafo_depth]))
 {
                ctxt_idx = IHEVC_CAB_CBF_LUMA_IDX;
                ctxt_idx += (trafo_depth == 0) ? 1 : 0;

                TRACE_CABAC_CTXT("cbf_luma", ps_cabac->u4_range, ctxt_idx);
                value = ihevcd_cabac_decode_bin(ps_cabac, ps_bitstrm, ctxt_idx);
                AEV_TRACE("cbf_luma", value, ps_cabac->u4_range);

                ps_codec->s_parse.s_cu.i1_cbf_luma = value;
 }
 else
 {
                ps_codec->s_parse.s_cu.i1_cbf_luma = 1;
 }

 /* Initialize ps_tu to default values */
 /* If required change this to WORD32 packed write */
            ps_tu->b1_cb_cbf = 0;
            ps_tu->b1_cr_cbf = 0;
            ps_tu->b1_y_cbf = 0;
            ps_tu->b4_pos_x = ((x0 - ctb_x_base) >> 2);
            ps_tu->b4_pos_y = ((y0 - ctb_y_base) >> 2);
            ps_tu->b1_transquant_bypass = ps_codec->s_parse.s_cu.i4_cu_transquant_bypass;
            ps_tu->b3_size = (log2_trafo_size - 2);
            ps_tu->b7_qp = ps_codec->s_parse.u4_qp;

            ps_tu->b6_luma_intra_mode = intra_pred_mode;
            ps_tu->b3_chroma_intra_mode_idx = ps_codec->s_parse.s_cu.i4_intra_chroma_pred_mode_idx;

 /* Section:7.3.12  Transform unit syntax inlined here */
 if(ps_codec->s_parse.s_cu.i1_cbf_luma ||
                            ps_codec->s_parse.s_cu.ai1_cbf_cb[trafo_depth] ||
                            ps_codec->s_parse.s_cu.ai1_cbf_cr[trafo_depth])
 {
                WORD32 intra_pred_mode_chroma;
 if(ps_pps->i1_cu_qp_delta_enabled_flag && !ps_codec->s_parse.i4_is_cu_qp_delta_coded)
 {


                    WORD32 c_max        = TU_MAX_QP_DELTA_ABS;
                    WORD32 ctxt_inc     = IHEVC_CAB_QP_DELTA_ABS;
                    WORD32 ctxt_inc_max = CTXT_MAX_QP_DELTA_ABS;

                    TRACE_CABAC_CTXT("cu_qp_delta_abs", ps_cabac->u4_range, ctxt_inc);
 /* qp_delta_abs is coded as combination of tunary and eg0 code  */
 /* See Table 9-32 and Table 9-37 for details on cu_qp_delta_abs */
                    cu_qp_delta_abs = ihevcd_cabac_decode_bins_tunary(ps_cabac,
                                                                      ps_bitstrm,
                                                                      c_max,
                                                                      ctxt_inc,
 0,
                                                                      ctxt_inc_max);
 if(cu_qp_delta_abs >= c_max)
 {
                        value = ihevcd_cabac_decode_bypass_bins_egk(ps_cabac, ps_bitstrm, 0);
                        cu_qp_delta_abs += value;

                     }
                     AEV_TRACE("cu_qp_delta_abs", cu_qp_delta_abs, ps_cabac->u4_range);
 
                     ps_codec->s_parse.i4_is_cu_qp_delta_coded = 1;
 
 
 if(cu_qp_delta_abs)
 {
                        value = ihevcd_cabac_decode_bypass_bin(ps_cabac, ps_bitstrm);
                        AEV_TRACE("cu_qp_delta_sign", value, ps_cabac->u4_range);

 if(value)

                             cu_qp_delta_abs = -cu_qp_delta_abs;
 
                     }
                     ps_codec->s_parse.s_cu.i4_cu_qp_delta = cu_qp_delta_abs;
 
                 }

 if(ps_codec->s_parse.s_cu.i1_cbf_luma)
 {
                    ps_tu->b1_y_cbf = 1;
                    ihevcd_parse_residual_coding(ps_codec, x0, y0, log2_trafo_size, 0, intra_pred_mode);
 }

 if(4 == ps_codec->s_parse.s_cu.i4_intra_chroma_pred_mode_idx)
                    intra_pred_mode_chroma = ps_codec->s_parse.s_cu.ai4_intra_luma_pred_mode[0];
 else
 {
                    intra_pred_mode_chroma = gau1_intra_pred_chroma_modes[ps_codec->s_parse.s_cu.i4_intra_chroma_pred_mode_idx];

 if(intra_pred_mode_chroma ==
                                    ps_codec->s_parse.s_cu.ai4_intra_luma_pred_mode[0])
 {
                        intra_pred_mode_chroma = INTRA_ANGULAR(34);
 }

 }
 if(log2_trafo_size > 2)
 {
 if(ps_codec->s_parse.s_cu.ai1_cbf_cb[trafo_depth])
 {
                        ps_tu->b1_cb_cbf = 1;
                        ihevcd_parse_residual_coding(ps_codec, x0, y0, log2_trafo_size - 1, 1, intra_pred_mode_chroma);
 }

 if(ps_codec->s_parse.s_cu.ai1_cbf_cr[trafo_depth])
 {
                        ps_tu->b1_cr_cbf = 1;
                        ihevcd_parse_residual_coding(ps_codec, x0, y0, log2_trafo_size - 1, 2, intra_pred_mode_chroma);
 }
 }
 else if(blk_idx == 3)
 {
 if(ps_codec->s_parse.s_cu.ai1_cbf_cb[trafo_depth])
 {
                        ps_tu->b1_cb_cbf = 1;
                        ihevcd_parse_residual_coding(ps_codec, cu_x_base, cu_y_base, log2_trafo_size, 1, intra_pred_mode_chroma);
 }

 if(ps_codec->s_parse.s_cu.ai1_cbf_cr[trafo_depth])
 {
                        ps_tu->b1_cr_cbf = 1;
                        ihevcd_parse_residual_coding(ps_codec, cu_x_base, cu_y_base, log2_trafo_size, 2, intra_pred_mode_chroma);
 }
 }
 else
 {
                    ps_tu->b3_chroma_intra_mode_idx = INTRA_PRED_CHROMA_IDX_NONE;
 }
 }
 else
 {
 if((3 != blk_idx) && (2 == log2_trafo_size))
 {
                    ps_tu->b3_chroma_intra_mode_idx = INTRA_PRED_CHROMA_IDX_NONE;
 }
 }

 /* Set the first TU in CU flag */
 {
 if((ps_codec->s_parse.s_cu.i4_pos_x << 3) == (ps_tu->b4_pos_x << 2) &&
 (ps_codec->s_parse.s_cu.i4_pos_y << 3) == (ps_tu->b4_pos_y << 2))
 {
                    ps_tu->b1_first_tu_in_cu = 1;
 }
 else
 {
                    ps_tu->b1_first_tu_in_cu = 0;
 }
 }
            ps_codec->s_parse.ps_tu++;
            ps_codec->s_parse.s_cu.i4_tu_cnt++;
            ps_codec->s_parse.i4_pic_tu_idx++;
 }
 }
 return ret;
}
