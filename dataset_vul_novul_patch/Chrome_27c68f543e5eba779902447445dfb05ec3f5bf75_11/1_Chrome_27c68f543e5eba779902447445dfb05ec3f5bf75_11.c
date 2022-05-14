size_t Vp9Parser::GetQIndex(const Vp9QuantizationParams& quant,
                            size_t segid) const {
  if (segmentation_.FeatureEnabled(segid, Vp9Segmentation::SEG_LVL_ALT_Q)) {
    int8_t feature_data =
        segmentation_.FeatureData(segid, Vp9Segmentation::SEG_LVL_ALT_Q);
    size_t q_index = segmentation_.abs_delta ? feature_data
                                             : quant.base_qindex + feature_data;
    return CLAMP_Q(q_index);
  }
  return quant.base_qindex;
}
