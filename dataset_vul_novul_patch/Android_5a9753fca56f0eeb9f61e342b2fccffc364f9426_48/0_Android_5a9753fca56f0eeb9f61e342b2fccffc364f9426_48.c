   void RunInvAccuracyCheck() {
     ACMRandom rnd(ACMRandom::DeterministicSeed());
     const int count_test_block = 1000;
    DECLARE_ALIGNED(16, int16_t, in[kNumCoeffs]);
    DECLARE_ALIGNED(16, tran_low_t, coeff[kNumCoeffs]);
    DECLARE_ALIGNED(16, uint8_t, dst[kNumCoeffs]);
    DECLARE_ALIGNED(16, uint8_t, src[kNumCoeffs]);
#if CONFIG_VP9_HIGHBITDEPTH
    DECLARE_ALIGNED(16, uint16_t, dst16[kNumCoeffs]);
    DECLARE_ALIGNED(16, uint16_t, src16[kNumCoeffs]);
#endif  // CONFIG_VP9_HIGHBITDEPTH
 
     for (int i = 0; i < count_test_block; ++i) {
       double out_r[kNumCoeffs];
 
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
#endif  // CONFIG_VP9_HIGHBITDEPTH
        }
       }
 
       reference_16x16_dct_2d(in, out_r);
       for (int j = 0; j < kNumCoeffs; ++j)
        coeff[j] = static_cast<tran_low_t>(round(out_r[j]));
 
      if (bit_depth_ == VPX_BITS_8) {
        ASM_REGISTER_STATE_CHECK(RunInvTxfm(coeff, dst, 16));
#if CONFIG_VP9_HIGHBITDEPTH
      } else {
        ASM_REGISTER_STATE_CHECK(RunInvTxfm(coeff, CONVERT_TO_BYTEPTR(dst16),
                                            16));
#endif  // CONFIG_VP9_HIGHBITDEPTH
      }
 
       for (int j = 0; j < kNumCoeffs; ++j) {
#if CONFIG_VP9_HIGHBITDEPTH
        const uint32_t diff =
            bit_depth_ == VPX_BITS_8 ? dst[j] - src[j] : dst16[j] - src16[j];
#else
         const uint32_t diff = dst[j] - src[j];
#endif  // CONFIG_VP9_HIGHBITDEPTH
         const uint32_t error = diff * diff;
         EXPECT_GE(1u, error)
             << "Error: 16x16 IDCT has error " << error
 << " at index " << j;

       }
     }
   }
