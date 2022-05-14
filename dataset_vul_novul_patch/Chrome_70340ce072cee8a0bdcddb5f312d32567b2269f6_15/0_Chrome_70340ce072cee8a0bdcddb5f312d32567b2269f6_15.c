bool VaapiVideoDecodeAccelerator::VaapiH264Accelerator::SubmitSlice(
    const H264PPS* pps,
    const H264SliceHeader* slice_hdr,
    const H264Picture::Vector& ref_pic_list0,
    const H264Picture::Vector& ref_pic_list1,
     const scoped_refptr<H264Picture>& pic,
     const uint8_t* data,
     size_t size) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
   VASliceParameterBufferH264 slice_param;
   memset(&slice_param, 0, sizeof(slice_param));
 
  slice_param.slice_data_size = slice_hdr->nalu_size;
  slice_param.slice_data_offset = 0;
  slice_param.slice_data_flag = VA_SLICE_DATA_FLAG_ALL;
  slice_param.slice_data_bit_offset = slice_hdr->header_bit_size;

#define SHDRToSP(a) slice_param.a = slice_hdr->a
  SHDRToSP(first_mb_in_slice);
  slice_param.slice_type = slice_hdr->slice_type % 5;
  SHDRToSP(direct_spatial_mv_pred_flag);

  SHDRToSP(num_ref_idx_l0_active_minus1);
  SHDRToSP(num_ref_idx_l1_active_minus1);
  SHDRToSP(cabac_init_idc);
  SHDRToSP(slice_qp_delta);
  SHDRToSP(disable_deblocking_filter_idc);
  SHDRToSP(slice_alpha_c0_offset_div2);
  SHDRToSP(slice_beta_offset_div2);

  if (((slice_hdr->IsPSlice() || slice_hdr->IsSPSlice()) &&
       pps->weighted_pred_flag) ||
      (slice_hdr->IsBSlice() && pps->weighted_bipred_idc == 1)) {
    SHDRToSP(luma_log2_weight_denom);
    SHDRToSP(chroma_log2_weight_denom);

    SHDRToSP(luma_weight_l0_flag);
    SHDRToSP(luma_weight_l1_flag);

    SHDRToSP(chroma_weight_l0_flag);
    SHDRToSP(chroma_weight_l1_flag);

    for (int i = 0; i <= slice_param.num_ref_idx_l0_active_minus1; ++i) {
      slice_param.luma_weight_l0[i] =
          slice_hdr->pred_weight_table_l0.luma_weight[i];
      slice_param.luma_offset_l0[i] =
          slice_hdr->pred_weight_table_l0.luma_offset[i];

      for (int j = 0; j < 2; ++j) {
        slice_param.chroma_weight_l0[i][j] =
            slice_hdr->pred_weight_table_l0.chroma_weight[i][j];
        slice_param.chroma_offset_l0[i][j] =
            slice_hdr->pred_weight_table_l0.chroma_offset[i][j];
      }
    }

    if (slice_hdr->IsBSlice()) {
      for (int i = 0; i <= slice_param.num_ref_idx_l1_active_minus1; ++i) {
        slice_param.luma_weight_l1[i] =
            slice_hdr->pred_weight_table_l1.luma_weight[i];
        slice_param.luma_offset_l1[i] =
            slice_hdr->pred_weight_table_l1.luma_offset[i];

        for (int j = 0; j < 2; ++j) {
          slice_param.chroma_weight_l1[i][j] =
              slice_hdr->pred_weight_table_l1.chroma_weight[i][j];
          slice_param.chroma_offset_l1[i][j] =
              slice_hdr->pred_weight_table_l1.chroma_offset[i][j];
        }
      }
    }
  }

  static_assert(
      arraysize(slice_param.RefPicList0) == arraysize(slice_param.RefPicList1),
      "Invalid RefPicList sizes");

  for (size_t i = 0; i < arraysize(slice_param.RefPicList0); ++i) {
    InitVAPicture(&slice_param.RefPicList0[i]);
    InitVAPicture(&slice_param.RefPicList1[i]);
  }

  for (size_t i = 0;
       i < ref_pic_list0.size() && i < arraysize(slice_param.RefPicList0);
       ++i) {
    if (ref_pic_list0[i])
      FillVAPicture(&slice_param.RefPicList0[i], ref_pic_list0[i]);
  }
  for (size_t i = 0;
       i < ref_pic_list1.size() && i < arraysize(slice_param.RefPicList1);
       ++i) {
    if (ref_pic_list1[i])
      FillVAPicture(&slice_param.RefPicList1[i], ref_pic_list1[i]);
  }

  if (!vaapi_wrapper_->SubmitBuffer(VASliceParameterBufferType,
                                    sizeof(slice_param), &slice_param))
    return false;

  void* non_const_ptr = const_cast<uint8_t*>(data);
  return vaapi_wrapper_->SubmitBuffer(VASliceDataBufferType, size,
                                      non_const_ptr);
}
