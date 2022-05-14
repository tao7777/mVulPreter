bool AudioRendererAlgorithm::OutputSlowerPlayback(uint8* dest) {
bool AudioRendererAlgorithm::OutputSlowerPlayback(uint8* dest,
                                                  int input_step,
                                                  int output_step) {
  // Ensure we don't run into OOB read/write situation.
  CHECK_LT(input_step, output_step);
   DCHECK_LT(index_into_window_, window_size_);
   DCHECK_LT(playback_rate_, 1.0);
   DCHECK_NE(playback_rate_, 0.0);

  if (audio_buffer_.forward_bytes() < bytes_per_frame_)
    return false;

   int bytes_to_crossfade = bytes_in_crossfade_;
   if (muted_ || bytes_to_crossfade > input_step)
     bytes_to_crossfade = 0;

  int intro_crossfade_begin = input_step - bytes_to_crossfade;

  int intro_crossfade_end = input_step;

  int outtro_crossfade_begin = output_step - bytes_to_crossfade;

  if (index_into_window_ < intro_crossfade_begin) {
    CopyWithAdvance(dest);
    index_into_window_ += bytes_per_frame_;
    return true;
  }

  if (index_into_window_ < intro_crossfade_end) {
    int offset = index_into_window_ - intro_crossfade_begin;
    uint8* place_to_copy = crossfade_buffer_.get() + offset;
    CopyWithoutAdvance(place_to_copy);
    CopyWithAdvance(dest);
    index_into_window_ += bytes_per_frame_;
    return true;
  }

  int audio_buffer_offset = index_into_window_ - intro_crossfade_end;

  if (audio_buffer_.forward_bytes() < audio_buffer_offset + bytes_per_frame_)
    return false;

  DCHECK_GE(index_into_window_, intro_crossfade_end);
  CopyWithoutAdvance(dest, audio_buffer_offset);

  if (index_into_window_ >= outtro_crossfade_begin) {
    int offset_into_crossfade_buffer =
        index_into_window_ - outtro_crossfade_begin;
    uint8* intro_frame_ptr =
        crossfade_buffer_.get() + offset_into_crossfade_buffer;
    OutputCrossfadedFrame(dest, intro_frame_ptr);
  }

  index_into_window_ += bytes_per_frame_;
  return true;
}
