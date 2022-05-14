void Vp9Parser::SetupLoopFilter() {
  if (!loop_filter_.filter_level)
    return;
  int scale = loop_filter_.filter_level < 32 ? 1 : 2;
  for (size_t i = 0; i < Vp9Segmentation::kNumSegments; ++i) {
    int level = loop_filter_.filter_level;
    if (segmentation_.FeatureEnabled(i, Vp9Segmentation::SEG_LVL_ALT_LF)) {
      int feature_data =
          segmentation_.FeatureData(i, Vp9Segmentation::SEG_LVL_ALT_LF);
      level = CLAMP_LF(segmentation_.abs_delta ? feature_data
                                               : level + feature_data);
    }
    if (!loop_filter_.mode_ref_delta_enabled) {
      memset(loop_filter_.lvl[i], level, sizeof(loop_filter_.lvl[i]));
    } else {
      loop_filter_.lvl[i][Vp9LoopFilter::VP9_FRAME_INTRA][0] = CLAMP_LF(
          level +
          loop_filter_.ref_deltas[Vp9LoopFilter::VP9_FRAME_INTRA] * scale);
      loop_filter_.lvl[i][Vp9LoopFilter::VP9_FRAME_INTRA][1] = 0;
      for (size_t type = Vp9LoopFilter::VP9_FRAME_LAST;
           type < Vp9LoopFilter::VP9_FRAME_MAX; ++type) {
        for (size_t mode = 0; mode < Vp9LoopFilter::kNumModeDeltas; ++mode) {
          loop_filter_.lvl[i][type][mode] =
              CLAMP_LF(level + loop_filter_.ref_deltas[type] * scale +
                       loop_filter_.mode_deltas[mode] * scale);
        }
      }
    }
  }
}
