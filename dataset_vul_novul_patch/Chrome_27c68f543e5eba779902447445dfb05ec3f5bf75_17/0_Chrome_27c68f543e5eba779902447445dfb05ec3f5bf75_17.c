void Vp9Parser::ReadLoopFilter() {
void Vp9Parser::ReadLoopFilter(Vp9LoopFilter* loop_filter) {
  loop_filter->filter_level = reader_.ReadLiteral(6);
  loop_filter->sharpness_level = reader_.ReadLiteral(3);

  loop_filter->mode_ref_delta_enabled = reader_.ReadBool();
  if (loop_filter->mode_ref_delta_enabled) {
    loop_filter->mode_ref_delta_update = reader_.ReadBool();
    if (loop_filter->mode_ref_delta_update) {
      for (size_t i = 0; i < Vp9LoopFilter::kNumRefDeltas; i++) {
        loop_filter->update_ref_deltas[i] = reader_.ReadBool();
        if (loop_filter->update_ref_deltas[i])
          loop_filter->ref_deltas[i] = reader_.ReadSignedLiteral(6);
       }
 
       for (size_t i = 0; i < Vp9LoopFilter::kNumModeDeltas; i++) {
        loop_filter->update_mode_deltas[i] = reader_.ReadBool();
        if (loop_filter->update_mode_deltas[i])
          loop_filter->mode_deltas[i] = reader_.ReadLiteral(6);
       }
     }
   }
}
