void Vp9Parser::ResetLoopfilter() {
  loop_filter_.mode_ref_delta_enabled = true;
  loop_filter_.mode_ref_delta_update = true;
  const int8_t default_ref_deltas[] = {1, 0, -1, -1};
  static_assert(
      arraysize(default_ref_deltas) == arraysize(loop_filter_.ref_deltas),
      "ref_deltas arrays of incorrect size");
  for (size_t i = 0; i < arraysize(loop_filter_.ref_deltas); ++i)
    loop_filter_.ref_deltas[i] = default_ref_deltas[i];
  memset(loop_filter_.mode_deltas, 0, sizeof(loop_filter_.mode_deltas));
}
