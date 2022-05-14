int AudioRendererAlgorithm::FillBuffer(
    uint8* dest, int requested_frames) {
  DCHECK_NE(bytes_per_frame_, 0);

   if (playback_rate_ == 0.0f)
     return 0;
 
   int total_frames_rendered = 0;
   uint8* output_ptr = dest;
   while (total_frames_rendered < requested_frames) {
     if (index_into_window_ == window_size_)
       ResetWindow();
 
     bool rendered_frame = true;
    if (playback_rate_ > 1.0)
      rendered_frame = OutputFasterPlayback(output_ptr);
    else if (playback_rate_ < 1.0)
      rendered_frame = OutputSlowerPlayback(output_ptr);
    else
       rendered_frame = OutputNormalPlayback(output_ptr);
 
     if (!rendered_frame) {
       needs_more_data_ = true;
      break;
    }

    output_ptr += bytes_per_frame_;
    total_frames_rendered++;
  }
  return total_frames_rendered;
}
