  virtual void PreEncodeFrameHook(libvpx_test::VideoSource *video) {
  //
  // Frame flags and layer id for temporal layers.
  // For two layers, test pattern is:
  //   1     3
  // 0    2     .....
  // LAST is updated on base/layer 0, GOLDEN  updated on layer 1.
  // Non-zero pattern_switch parameter means pattern will switch to
  // not using LAST for frame_num >= pattern_switch.
  int SetFrameFlags(int frame_num,
                    int num_temp_layers,
                    int pattern_switch) {
    int frame_flags = 0;
    if (num_temp_layers == 2) {
        if (frame_num % 2 == 0) {
          if (frame_num < pattern_switch || pattern_switch == 0) {
            // Layer 0: predict from LAST and ARF, update LAST.
            frame_flags = VP8_EFLAG_NO_REF_GF |
                          VP8_EFLAG_NO_UPD_GF |
                          VP8_EFLAG_NO_UPD_ARF;
          } else {
            // Layer 0: predict from GF and ARF, update GF.
            frame_flags = VP8_EFLAG_NO_REF_LAST |
                          VP8_EFLAG_NO_UPD_LAST |
                          VP8_EFLAG_NO_UPD_ARF;
          }
        } else {
          if (frame_num < pattern_switch || pattern_switch == 0) {
            // Layer 1: predict from L, GF, and ARF, update GF.
            frame_flags = VP8_EFLAG_NO_UPD_ARF |
                          VP8_EFLAG_NO_UPD_LAST;
          } else {
            // Layer 1: predict from GF and ARF, update GF.
            frame_flags = VP8_EFLAG_NO_REF_LAST |
                          VP8_EFLAG_NO_UPD_LAST |
                          VP8_EFLAG_NO_UPD_ARF;
          }
        }
    }
    return frame_flags;
  }

  virtual void PreEncodeFrameHook(libvpx_test::VideoSource *video,
                                  ::libvpx_test::Encoder *encoder) {
     frame_flags_ &= ~(VP8_EFLAG_NO_UPD_LAST |
                       VP8_EFLAG_NO_UPD_GF |
                       VP8_EFLAG_NO_UPD_ARF);
    // For temporal layer case.
    if (cfg_.ts_number_layers > 1) {
      frame_flags_ = SetFrameFlags(video->frame(),
                                   cfg_.ts_number_layers,
                                   pattern_switch_);
       for (unsigned int i = 0; i < droppable_nframes_; ++i) {
         if (droppable_frames_[i] == video->frame()) {
          std::cout << "Encoding droppable frame: "
                     << droppable_frames_[i] << "\n";
         }
       }
    } else {
       if (droppable_nframes_ > 0 &&
         (cfg_.g_pass == VPX_RC_LAST_PASS || cfg_.g_pass == VPX_RC_ONE_PASS)) {
         for (unsigned int i = 0; i < droppable_nframes_; ++i) {
           if (droppable_frames_[i] == video->frame()) {
             std::cout << "Encoding droppable frame: "
                       << droppable_frames_[i] << "\n";
             frame_flags_ |= (VP8_EFLAG_NO_UPD_LAST |
                              VP8_EFLAG_NO_UPD_GF |
                              VP8_EFLAG_NO_UPD_ARF);
             return;
           }
         }
       }
     }
   }
