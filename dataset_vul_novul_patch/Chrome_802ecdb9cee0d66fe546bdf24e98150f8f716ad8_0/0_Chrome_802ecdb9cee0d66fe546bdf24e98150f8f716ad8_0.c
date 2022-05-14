bool AudioRendererAlgorithm::OutputFasterPlayback(uint8* dest) {
bool AudioRendererAlgorithm::OutputFasterPlayback(uint8* dest,
                                                  int input_step,
                                                  int output_step) {
  // Ensure we don't run into OOB read/write situation.
  CHECK_GT(input_step, output_step);
   DCHECK_LT(index_into_window_, window_size_);
   DCHECK_GT(playback_rate_, 1.0);
 
  if (audio_buffer_.forward_bytes() < bytes_per_frame_)
    return false;

   int bytes_to_crossfade = bytes_in_crossfade_;
   if (muted_ || bytes_to_crossfade > output_step)
     bytes_to_crossfade = 0;

  int outtro_crossfade_begin = output_step - bytes_to_crossfade;

  int outtro_crossfade_end = output_step;

  int intro_crossfade_begin = input_step - bytes_to_crossfade;

  if (index_into_window_ < outtro_crossfade_begin) {
    CopyWithAdvance(dest);
    index_into_window_ += bytes_per_frame_;
    return true;
  }

  while (index_into_window_ < outtro_crossfade_end) {
    if (audio_buffer_.forward_bytes() < bytes_per_frame_)
      return false;

    DCHECK_GT(bytes_to_crossfade, 0);
    uint8* place_to_copy = crossfade_buffer_.get() +
        (index_into_window_ - outtro_crossfade_begin);
    CopyWithAdvance(place_to_copy);
    index_into_window_ += bytes_per_frame_;
  }

  while (index_into_window_ < intro_crossfade_begin) {
    if (audio_buffer_.forward_bytes() < bytes_per_frame_)
      return false;

    DropFrame();
    index_into_window_ += bytes_per_frame_;
  }

  if (audio_buffer_.forward_bytes() < bytes_per_frame_)
    return false;

  if (bytes_to_crossfade == 0) {
    DCHECK_EQ(index_into_window_, window_size_);
    return false;
  }

  DCHECK_LT(index_into_window_, window_size_);
  int offset_into_buffer = index_into_window_ - intro_crossfade_begin;
  memcpy(dest, crossfade_buffer_.get() + offset_into_buffer,
         bytes_per_frame_);
  scoped_array<uint8> intro_frame_ptr(new uint8[bytes_per_frame_]);
  audio_buffer_.Read(intro_frame_ptr.get(), bytes_per_frame_);
  OutputCrossfadedFrame(dest, intro_frame_ptr.get());
  index_into_window_ += bytes_per_frame_;
   return true;
 }
