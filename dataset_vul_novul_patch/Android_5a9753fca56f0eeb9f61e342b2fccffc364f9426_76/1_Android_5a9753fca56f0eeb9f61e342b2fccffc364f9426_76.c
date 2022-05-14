  void RunInvAccuracyCheck() {
     ACMRandom rnd(ACMRandom::DeterministicSeed());
     const int count_test_block = 1000;
    DECLARE_ALIGNED_ARRAY(16, int16_t, in, kNumCoeffs);
    DECLARE_ALIGNED_ARRAY(16, int16_t, coeff, kNumCoeffs);
    DECLARE_ALIGNED_ARRAY(16, uint8_t, dst, kNumCoeffs);
    DECLARE_ALIGNED_ARRAY(16, uint8_t, src, kNumCoeffs);
 
     for (int i = 0; i < count_test_block; ++i) {
       for (int j = 0; j < kNumCoeffs; ++j) {
        src[j] = rnd.Rand8();
        dst[j] = rnd.Rand8();
        in[j] = src[j] - dst[j];
       }
 
       fwd_txfm_ref(in, coeff, pitch_, tx_type_);
 
      REGISTER_STATE_CHECK(RunInvTxfm(coeff, dst, pitch_));
 
       for (int j = 0; j < kNumCoeffs; ++j) {
         const uint32_t diff = dst[j] - src[j];
         const uint32_t error = diff * diff;
        EXPECT_GE(1u, error)
            << "Error: 16x16 IDCT has error " << error
             << " at index " << j;
       }
     }
 }
