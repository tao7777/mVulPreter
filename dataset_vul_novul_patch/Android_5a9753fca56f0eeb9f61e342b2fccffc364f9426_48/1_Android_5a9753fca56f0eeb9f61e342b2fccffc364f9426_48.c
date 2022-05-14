   void RunInvAccuracyCheck() {
     ACMRandom rnd(ACMRandom::DeterministicSeed());
     const int count_test_block = 1000;
    DECLARE_ALIGNED_ARRAY(16, int16_t, in, kNumCoeffs);
    DECLARE_ALIGNED_ARRAY(16, int16_t, coeff, kNumCoeffs);
    DECLARE_ALIGNED_ARRAY(16, uint8_t, dst, kNumCoeffs);
    DECLARE_ALIGNED_ARRAY(16, uint8_t, src, kNumCoeffs);
 
     for (int i = 0; i < count_test_block; ++i) {
       double out_r[kNumCoeffs];
 
       for (int j = 0; j < kNumCoeffs; ++j) {
        src[j] = rnd.Rand8();
        dst[j] = rnd.Rand8();
        in[j] = src[j] - dst[j];
       }
 
       reference_16x16_dct_2d(in, out_r);
       for (int j = 0; j < kNumCoeffs; ++j)
        coeff[j] = round(out_r[j]);
 
      REGISTER_STATE_CHECK(RunInvTxfm(coeff, dst, 16));
 
       for (int j = 0; j < kNumCoeffs; ++j) {
         const uint32_t diff = dst[j] - src[j];
         const uint32_t error = diff * diff;
         EXPECT_GE(1u, error)
             << "Error: 16x16 IDCT has error " << error
 << " at index " << j;

       }
     }
   }
