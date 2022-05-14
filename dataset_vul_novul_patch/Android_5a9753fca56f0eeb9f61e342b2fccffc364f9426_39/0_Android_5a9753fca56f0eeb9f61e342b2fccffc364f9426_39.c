   virtual void SetUp() {
     InitializeConfig();
    SetMode(encoding_mode_);
    if (encoding_mode_ != ::libvpx_test::kRealTime) {
      cfg_.g_lag_in_frames = 25;
      cfg_.rc_end_usage = VPX_VBR;
    } else {
      cfg_.g_lag_in_frames = 0;
      cfg_.rc_end_usage = VPX_CBR;
    }
  }

  virtual void BeginPassHook(unsigned int /*pass*/) {
    min_psnr_ = kMaxPSNR;
   }
