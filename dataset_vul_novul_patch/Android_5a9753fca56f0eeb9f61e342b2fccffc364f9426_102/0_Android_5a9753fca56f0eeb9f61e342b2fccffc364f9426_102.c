   virtual void SetUp() {
    if (bd_ == -1) {
      use_high_bit_depth_ = false;
      bit_depth_ = VPX_BITS_8;
      source_data_ = source_data8_;
      reference_data_ = reference_data8_;
      second_pred_ = second_pred8_;
#if CONFIG_VP9_HIGHBITDEPTH
    } else {
      use_high_bit_depth_ = true;
      bit_depth_ = static_cast<vpx_bit_depth_t>(bd_);
      source_data_ = CONVERT_TO_BYTEPTR(source_data16_);
      reference_data_ = CONVERT_TO_BYTEPTR(reference_data16_);
      second_pred_ = CONVERT_TO_BYTEPTR(second_pred16_);
#endif  // CONFIG_VP9_HIGHBITDEPTH
    }
    mask_ = (1 << bit_depth_) - 1;
     source_stride_ = (width_ + 31) & ~31;
     reference_stride_ = width_ * 2;
     rnd_.Reset(ACMRandom::DeterministicSeed());
   }
