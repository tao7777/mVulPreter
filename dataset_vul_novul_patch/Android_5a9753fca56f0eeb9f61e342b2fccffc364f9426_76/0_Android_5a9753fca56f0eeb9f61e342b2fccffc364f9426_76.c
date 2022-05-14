  void RunInvAccuracyCheck() {
  void RunInvAccuracyCheck(int limit) {
     ACMRandom rnd(ACMRandom::DeterministicSeed());
     const int count_test_block = 1000;
    DECLARE_ALIGNED(16, int16_t, in[kNumCoeffs]);
    DECLARE_ALIGNED(16, tran_low_t, coeff[kNumCoeffs]);
    DECLARE_ALIGNED(16, uint8_t, dst[kNumCoeffs]);
    DECLARE_ALIGNED(16, uint8_t, src[kNumCoeffs]);
#if CONFIG_VP9_HIGHBITDEPTH
    DECLARE_ALIGNED(16, uint16_t, dst16[kNumCoeffs]);
    DECLARE_ALIGNED(16, uint16_t, src16[kNumCoeffs]);
#endif
 
     for (int i = 0; i < count_test_block; ++i) {
      // Initialize a test block with input range [-mask_, mask_].
       for (int j = 0; j < kNumCoeffs; ++j) {
        if (bit_depth_ == VPX_BITS_8) {
          src[j] = rnd.Rand8();
          dst[j] = rnd.Rand8();
          in[j] = src[j] - dst[j];
#if CONFIG_VP9_HIGHBITDEPTH
        } else {
          src16[j] = rnd.Rand16() & mask_;
          dst16[j] = rnd.Rand16() & mask_;
          in[j] = src16[j] - dst16[j];
#endif
        }
       }
 
       fwd_txfm_ref(in, coeff, pitch_, tx_type_);
 
      if (bit_depth_ == VPX_BITS_8) {
        ASM_REGISTER_STATE_CHECK(RunInvTxfm(coeff, dst, pitch_));
#if CONFIG_VP9_HIGHBITDEPTH
      } else {
        ASM_REGISTER_STATE_CHECK(RunInvTxfm(coeff, CONVERT_TO_BYTEPTR(dst16),
                                            pitch_));
#endif
      }
 
       for (int j = 0; j < kNumCoeffs; ++j) {
#if CONFIG_VP9_HIGHBITDEPTH
        const uint32_t diff =
            bit_depth_ == VPX_BITS_8 ? dst[j] - src[j] : dst16[j] - src16[j];
#else
         const uint32_t diff = dst[j] - src[j];
#endif
         const uint32_t error = diff * diff;
        EXPECT_GE(static_cast<uint32_t>(limit), error)
            << "Error: 4x4 IDCT has error " << error
             << " at index " << j;
       }
     }
 }
