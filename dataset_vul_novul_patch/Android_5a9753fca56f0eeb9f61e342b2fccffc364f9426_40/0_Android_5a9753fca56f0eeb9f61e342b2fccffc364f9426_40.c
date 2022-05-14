   virtual void PreEncodeFrameHook(::libvpx_test::VideoSource *video,
                                   ::libvpx_test::Encoder *encoder) {
    if (video->frame() == 0)
      encoder->Control(VP8E_SET_NOISE_SENSITIVITY, denoiser_on_);

    if (denoiser_offon_test_) {
      ASSERT_GT(denoiser_offon_period_, 0)
          << "denoiser_offon_period_ is not positive.";
      if ((video->frame() + 1) % denoiser_offon_period_ == 0) {
        // Flip denoiser_on_ periodically
        denoiser_on_ ^= 1;
      }
      encoder->Control(VP8E_SET_NOISE_SENSITIVITY, denoiser_on_);
    }

     const vpx_rational_t tb = video->timebase();
     timebase_ = static_cast<double>(tb.num) / tb.den;
     duration_ = 0;
 }
