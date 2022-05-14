   virtual void PreEncodeFrameHook(::libvpx_test::VideoSource *video,
                                   ::libvpx_test::Encoder *encoder) {
     const vpx_rational_t tb = video->timebase();
     timebase_ = static_cast<double>(tb.num) / tb.den;
     duration_ = 0;
 }
