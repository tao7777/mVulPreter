int AudioRendererAlgorithm::FillBuffer(
    uint8* dest, int requested_frames) {
  DCHECK_NE(bytes_per_frame_, 0);

   if (playback_rate_ == 0.0f)
     return 0;
 
  int slower_step = ceil(window_size_ * playback_rate_);
  int faster_step = ceil(window_size_ / playback_rate_);
  AlignToFrameBoundary(&slower_step);
  AlignToFrameBoundary(&faster_step);

   int total_frames_rendered = 0;
   uint8* output_ptr = dest;
   while (total_frames_rendered < requested_frames) {
     if (index_into_window_ == window_size_)
       ResetWindow();
 
     bool rendered_frame = true;
    if (window_size_ > faster_step) {
      rendered_frame = OutputFasterPlayback(
          output_ptr, window_size_, faster_step);
    } else if (slower_step < window_size_) {
      rendered_frame = OutputSlowerPlayback(
          output_ptr, slower_step, window_size_);
    } else {
       rendered_frame = OutputNormalPlayback(output_ptr);
    }
 
     if (!rendered_frame) {
       needs_more_data_ = true;
      break;
    }

    output_ptr += bytes_per_frame_;
    total_frames_rendered++;
  }
  return total_frames_rendered;
}
