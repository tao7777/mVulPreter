  void RunAccuracyCheck() {
  void RunAccuracyCheck(int limit) {
     ACMRandom rnd(ACMRandom::DeterministicSeed());
     uint32_t max_error = 0;
     int64_t total_error = 0;
     const int count_test_block = 10000;
     for (int i = 0; i < count_test_block; ++i) {
      DECLARE_ALIGNED(16, int16_t, test_input_block[kNumCoeffs]);
      DECLARE_ALIGNED(16, tran_low_t, test_temp_block[kNumCoeffs]);
      DECLARE_ALIGNED(16, uint8_t, dst[kNumCoeffs]);
      DECLARE_ALIGNED(16, uint8_t, src[kNumCoeffs]);
#if CONFIG_VP9_HIGHBITDEPTH
      DECLARE_ALIGNED(16, uint16_t, dst16[kNumCoeffs]);
      DECLARE_ALIGNED(16, uint16_t, src16[kNumCoeffs]);
#endif
 
       for (int j = 0; j < kNumCoeffs; ++j) {
        if (bit_depth_ == VPX_BITS_8) {
          src[j] = rnd.Rand8();
          dst[j] = rnd.Rand8();
          test_input_block[j] = src[j] - dst[j];
#if CONFIG_VP9_HIGHBITDEPTH
        } else {
          src16[j] = rnd.Rand16() & mask_;
          dst16[j] = rnd.Rand16() & mask_;
          test_input_block[j] = src16[j] - dst16[j];
#endif
        }
       }
 
      ASM_REGISTER_STATE_CHECK(RunFwdTxfm(test_input_block,
                                          test_temp_block, pitch_));
      if (bit_depth_ == VPX_BITS_8) {
        ASM_REGISTER_STATE_CHECK(RunInvTxfm(test_temp_block, dst, pitch_));
#if CONFIG_VP9_HIGHBITDEPTH
      } else {
        ASM_REGISTER_STATE_CHECK(RunInvTxfm(test_temp_block,
                                            CONVERT_TO_BYTEPTR(dst16), pitch_));
#endif
      }
 
       for (int j = 0; j < kNumCoeffs; ++j) {
#if CONFIG_VP9_HIGHBITDEPTH
        const uint32_t diff =
            bit_depth_ == VPX_BITS_8 ? dst[j] - src[j] : dst16[j] - src16[j];
#else
        ASSERT_EQ(VPX_BITS_8, bit_depth_);
         const uint32_t diff = dst[j] - src[j];
#endif
         const uint32_t error = diff * diff;
         if (max_error < error)
           max_error = error;
        total_error += error;

       }
     }
 
    EXPECT_GE(static_cast<uint32_t>(limit), max_error)
        << "Error: 4x4 FHT/IHT has an individual round trip error > "
        << limit;
 
    EXPECT_GE(count_test_block * limit, total_error)
        << "Error: 4x4 FHT/IHT has average round trip error > " << limit
        << " per block";
   }
