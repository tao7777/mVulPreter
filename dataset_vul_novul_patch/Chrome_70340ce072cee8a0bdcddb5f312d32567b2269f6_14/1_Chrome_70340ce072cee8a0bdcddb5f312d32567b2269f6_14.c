bool VaapiVideoDecodeAccelerator::VaapiH264Accelerator::SubmitFrameMetadata(
    const H264SPS* sps,
    const H264PPS* pps,
    const H264DPB& dpb,
    const H264Picture::Vector& ref_pic_listp0,
     const H264Picture::Vector& ref_pic_listb0,
     const H264Picture::Vector& ref_pic_listb1,
     const scoped_refptr<H264Picture>& pic) {
   VAPictureParameterBufferH264 pic_param;
   memset(&pic_param, 0, sizeof(pic_param));
 
#define FROM_SPS_TO_PP(a) pic_param.a = sps->a
#define FROM_SPS_TO_PP2(a, b) pic_param.b = sps->a
  FROM_SPS_TO_PP2(pic_width_in_mbs_minus1, picture_width_in_mbs_minus1);
  FROM_SPS_TO_PP2(pic_height_in_map_units_minus1, picture_height_in_mbs_minus1);
  FROM_SPS_TO_PP(bit_depth_luma_minus8);
  FROM_SPS_TO_PP(bit_depth_chroma_minus8);
#undef FROM_SPS_TO_PP
#undef FROM_SPS_TO_PP2

#define FROM_SPS_TO_PP_SF(a) pic_param.seq_fields.bits.a = sps->a
#define FROM_SPS_TO_PP_SF2(a, b) pic_param.seq_fields.bits.b = sps->a
  FROM_SPS_TO_PP_SF(chroma_format_idc);
  FROM_SPS_TO_PP_SF2(separate_colour_plane_flag,
                     residual_colour_transform_flag);
  FROM_SPS_TO_PP_SF(gaps_in_frame_num_value_allowed_flag);
  FROM_SPS_TO_PP_SF(frame_mbs_only_flag);
  FROM_SPS_TO_PP_SF(mb_adaptive_frame_field_flag);
  FROM_SPS_TO_PP_SF(direct_8x8_inference_flag);
  pic_param.seq_fields.bits.MinLumaBiPredSize8x8 = (sps->level_idc >= 31);
  FROM_SPS_TO_PP_SF(log2_max_frame_num_minus4);
  FROM_SPS_TO_PP_SF(pic_order_cnt_type);
  FROM_SPS_TO_PP_SF(log2_max_pic_order_cnt_lsb_minus4);
  FROM_SPS_TO_PP_SF(delta_pic_order_always_zero_flag);
#undef FROM_SPS_TO_PP_SF
#undef FROM_SPS_TO_PP_SF2

#define FROM_PPS_TO_PP(a) pic_param.a = pps->a
  FROM_PPS_TO_PP(pic_init_qp_minus26);
  FROM_PPS_TO_PP(pic_init_qs_minus26);
  FROM_PPS_TO_PP(chroma_qp_index_offset);
  FROM_PPS_TO_PP(second_chroma_qp_index_offset);
#undef FROM_PPS_TO_PP

#define FROM_PPS_TO_PP_PF(a) pic_param.pic_fields.bits.a = pps->a
#define FROM_PPS_TO_PP_PF2(a, b) pic_param.pic_fields.bits.b = pps->a
  FROM_PPS_TO_PP_PF(entropy_coding_mode_flag);
  FROM_PPS_TO_PP_PF(weighted_pred_flag);
  FROM_PPS_TO_PP_PF(weighted_bipred_idc);
  FROM_PPS_TO_PP_PF(transform_8x8_mode_flag);

  pic_param.pic_fields.bits.field_pic_flag = 0;
  FROM_PPS_TO_PP_PF(constrained_intra_pred_flag);
  FROM_PPS_TO_PP_PF2(bottom_field_pic_order_in_frame_present_flag,
                     pic_order_present_flag);
  FROM_PPS_TO_PP_PF(deblocking_filter_control_present_flag);
  FROM_PPS_TO_PP_PF(redundant_pic_cnt_present_flag);
  pic_param.pic_fields.bits.reference_pic_flag = pic->ref;
#undef FROM_PPS_TO_PP_PF
#undef FROM_PPS_TO_PP_PF2

  pic_param.frame_num = pic->frame_num;

  InitVAPicture(&pic_param.CurrPic);
  FillVAPicture(&pic_param.CurrPic, pic);

  for (int i = 0; i < 16; ++i)
    InitVAPicture(&pic_param.ReferenceFrames[i]);

  FillVARefFramesFromDPB(dpb, pic_param.ReferenceFrames,
                         arraysize(pic_param.ReferenceFrames));

  pic_param.num_ref_frames = sps->max_num_ref_frames;

  if (!vaapi_wrapper_->SubmitBuffer(VAPictureParameterBufferType,
                                    sizeof(pic_param), &pic_param))
    return false;

  VAIQMatrixBufferH264 iq_matrix_buf;
  memset(&iq_matrix_buf, 0, sizeof(iq_matrix_buf));

  if (pps->pic_scaling_matrix_present_flag) {
    for (int i = 0; i < 6; ++i) {
      for (int j = 0; j < 16; ++j)
        iq_matrix_buf.ScalingList4x4[i][kZigzagScan4x4[j]] =
            pps->scaling_list4x4[i][j];
    }

    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 64; ++j)
        iq_matrix_buf.ScalingList8x8[i][kZigzagScan8x8[j]] =
            pps->scaling_list8x8[i][j];
    }
  } else {
    for (int i = 0; i < 6; ++i) {
      for (int j = 0; j < 16; ++j)
        iq_matrix_buf.ScalingList4x4[i][kZigzagScan4x4[j]] =
            sps->scaling_list4x4[i][j];
    }

    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 64; ++j)
        iq_matrix_buf.ScalingList8x8[i][kZigzagScan8x8[j]] =
            sps->scaling_list8x8[i][j];
    }
  }

  return vaapi_wrapper_->SubmitBuffer(VAIQMatrixBufferType,
                                      sizeof(iq_matrix_buf), &iq_matrix_buf);
}
