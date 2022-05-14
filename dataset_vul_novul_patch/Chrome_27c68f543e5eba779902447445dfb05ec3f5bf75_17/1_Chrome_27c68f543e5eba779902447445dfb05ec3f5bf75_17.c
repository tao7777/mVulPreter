void Vp9Parser::ReadLoopFilter() {
  loop_filter_.filter_level = reader_.ReadLiteral(6);
  loop_filter_.sharpness_level = reader_.ReadLiteral(3);
  loop_filter_.mode_ref_delta_update = false;
  loop_filter_.mode_ref_delta_enabled = reader_.ReadBool();
  if (loop_filter_.mode_ref_delta_enabled) {
    loop_filter_.mode_ref_delta_update = reader_.ReadBool();
    if (loop_filter_.mode_ref_delta_update) {
      for (size_t i = 0; i < Vp9LoopFilter::VP9_FRAME_MAX; i++) {
        loop_filter_.update_ref_deltas[i] = reader_.ReadBool();
        if (loop_filter_.update_ref_deltas[i])
          loop_filter_.ref_deltas[i] = reader_.ReadSignedLiteral(6);
       }
 
       for (size_t i = 0; i < Vp9LoopFilter::kNumModeDeltas; i++) {
        loop_filter_.update_mode_deltas[i] = reader_.ReadBool();
        if (loop_filter_.update_mode_deltas[i])
          loop_filter_.mode_deltas[i] = reader_.ReadLiteral(6);
       }
     }
   }
}
