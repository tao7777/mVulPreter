void Vp9Parser::SetupSegmentationDequant(const Vp9QuantizationParams& quant) {
  if (segmentation_.enabled) {
    for (size_t i = 0; i < Vp9Segmentation::kNumSegments; ++i) {
      const size_t q_index = GetQIndex(quant, i);
      segmentation_.y_dequant[i][0] =
          kDcQLookup[CLAMP_Q(q_index + quant.y_dc_delta)];
      segmentation_.y_dequant[i][1] = kAcQLookup[CLAMP_Q(q_index)];
      segmentation_.uv_dequant[i][0] =
          kDcQLookup[CLAMP_Q(q_index + quant.uv_dc_delta)];
      segmentation_.uv_dequant[i][1] =
          kAcQLookup[CLAMP_Q(q_index + quant.uv_ac_delta)];
    }
  } else {
    const size_t q_index = quant.base_qindex;
    segmentation_.y_dequant[0][0] =
        kDcQLookup[CLAMP_Q(q_index + quant.y_dc_delta)];
    segmentation_.y_dequant[0][1] = kAcQLookup[CLAMP_Q(q_index)];
    segmentation_.uv_dequant[0][0] =
        kDcQLookup[CLAMP_Q(q_index + quant.uv_dc_delta)];
    segmentation_.uv_dequant[0][1] =
        kAcQLookup[CLAMP_Q(q_index + quant.uv_ac_delta)];
  }
}
