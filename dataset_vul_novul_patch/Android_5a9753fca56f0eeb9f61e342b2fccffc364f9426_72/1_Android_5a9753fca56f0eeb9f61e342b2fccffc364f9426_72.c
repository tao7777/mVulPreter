  void RunAccuracyCheck() {
     ACMRandom rnd(ACMRandom::DeterministicSeed());
     uint32_t max_error = 0;
     int64_t total_error = 0;
     const int count_test_block = 10000;
     for (int i = 0; i < count_test_block; ++i) {
      DECLARE_ALIGNED_ARRAY(16, int16_t, test_input_block, kNumCoeffs);
      DECLARE_ALIGNED_ARRAY(16, int16_t, test_temp_block, kNumCoeffs);
      DECLARE_ALIGNED_ARRAY(16, uint8_t, dst, kNumCoeffs);
      DECLARE_ALIGNED_ARRAY(16, uint8_t, src, kNumCoeffs);
 
       for (int j = 0; j < kNumCoeffs; ++j) {
        src[j] = rnd.Rand8();
        dst[j] = rnd.Rand8();
        test_input_block[j] = src[j] - dst[j];
       }
 
      REGISTER_STATE_CHECK(RunFwdTxfm(test_input_block,
                                      test_temp_block, pitch_));
      REGISTER_STATE_CHECK(RunInvTxfm(test_temp_block, dst, pitch_));
 
       for (int j = 0; j < kNumCoeffs; ++j) {
         const uint32_t diff = dst[j] - src[j];
         const uint32_t error = diff * diff;
         if (max_error < error)
           max_error = error;
        total_error += error;

       }
     }
 
    EXPECT_GE(1u, max_error)
        << "Error: 4x4 FHT/IHT has an individual round trip error > 1";
 
    EXPECT_GE(count_test_block , total_error)
        << "Error: 4x4 FHT/IHT has average round trip error > 1 per block";
   }
