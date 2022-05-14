  virtual void PreEncodeFrameHook(libvpx_test::VideoSource *video) {
     frame_flags_ &= ~(VP8_EFLAG_NO_UPD_LAST |
                       VP8_EFLAG_NO_UPD_GF |
                       VP8_EFLAG_NO_UPD_ARF);
    if (droppable_nframes_ > 0 &&
        (cfg_.g_pass == VPX_RC_LAST_PASS || cfg_.g_pass == VPX_RC_ONE_PASS)) {
       for (unsigned int i = 0; i < droppable_nframes_; ++i) {
         if (droppable_frames_[i] == video->frame()) {
          std::cout << "             Encoding droppable frame: "
                     << droppable_frames_[i] << "\n";
          frame_flags_ |= (VP8_EFLAG_NO_UPD_LAST |
                           VP8_EFLAG_NO_UPD_GF |
                           VP8_EFLAG_NO_UPD_ARF);
          return;
         }
       }
     }
   }
