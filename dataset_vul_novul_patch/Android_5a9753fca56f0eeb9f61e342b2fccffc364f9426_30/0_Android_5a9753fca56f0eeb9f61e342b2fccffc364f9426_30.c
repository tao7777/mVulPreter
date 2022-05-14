   virtual void SetUp() {
     UUT_ = GET_PARAM(2);
#if CONFIG_VP9_HIGHBITDEPTH
    if (UUT_->use_highbd_ != 0)
      mask_ = (1 << UUT_->use_highbd_) - 1;
    else
      mask_ = 255;
#endif
     /* Set up guard blocks for an inner block centered in the outer block */
     for (int i = 0; i < kOutputBufferSize; ++i) {
       if (IsIndexInBorder(i))
        output_[i] = 255;
 else
        output_[i] = 0;

     }
 
     ::libvpx_test::ACMRandom prng;
    for (int i = 0; i < kInputBufferSize; ++i) {
      if (i & 1) {
        input_[i] = 255;
#if CONFIG_VP9_HIGHBITDEPTH
        input16_[i] = mask_;
#endif
      } else {
        input_[i] = prng.Rand8Extremes();
#if CONFIG_VP9_HIGHBITDEPTH
        input16_[i] = prng.Rand16() & mask_;
#endif
      }
    }
   }
