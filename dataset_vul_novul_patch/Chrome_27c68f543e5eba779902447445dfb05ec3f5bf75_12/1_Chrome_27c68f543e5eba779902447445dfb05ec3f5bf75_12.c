bool Vp9FrameHeader::IsKeyframe() const {
  return !show_existing_frame && frame_type == KEYFRAME;
}
