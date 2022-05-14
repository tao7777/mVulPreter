   virtual void PreEncodeFrameHook(::libvpx_test::VideoSource *video,
                                   ::libvpx_test::Encoder *encoder) {
     if (video->frame() == 1) {
       encoder->Control(VP8E_SET_CPUUSED, set_cpu_used_);
      encoder->Control(VP8E_SET_ENABLEAUTOALTREF, 1);
      encoder->Control(VP8E_SET_ARNR_MAXFRAMES, 7);
      encoder->Control(VP8E_SET_ARNR_STRENGTH, 5);
      encoder->Control(VP8E_SET_ARNR_TYPE, 3);
     }
   }
