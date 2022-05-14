   virtual void PreEncodeFrameHook(::libvpx_test::VideoSource *video,
                                   ::libvpx_test::Encoder *encoder) {
    if (video->frame() == 1) {
       encoder->Control(VP8E_SET_CPUUSED, set_cpu_used_);
     }
     if (cfg_.ts_number_layers > 1) {
      if (video->frame() == 1) {
         encoder->Control(VP9E_SET_SVC, 1);
       }
      vpx_svc_layer_id_t layer_id = {0, 0};
       layer_id.spatial_layer_id = 0;
       frame_flags_ = SetFrameFlags(video->frame(), cfg_.ts_number_layers);
       layer_id.temporal_layer_id = SetLayerId(video->frame(),
                                               cfg_.ts_number_layers);
      if (video->frame() > 0) {
       encoder->Control(VP9E_SET_SVC_LAYER_ID, &layer_id);
      }
     }
     const vpx_rational_t tb = video->timebase();
     timebase_ = static_cast<double>(tb.num) / tb.den;
    duration_ = 0;
 }
