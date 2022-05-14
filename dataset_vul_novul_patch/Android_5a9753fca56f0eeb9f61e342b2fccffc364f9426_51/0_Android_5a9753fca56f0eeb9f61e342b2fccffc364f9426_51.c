   void RunMemCheck() {
     ACMRandom rnd(ACMRandom::DeterministicSeed());
     const int count_test_block = 1000;
    DECLARE_ALIGNED(16, int16_t, input_extreme_block[kNumCoeffs]);
    DECLARE_ALIGNED(16, tran_low_t, output_ref_block[kNumCoeffs]);
    DECLARE_ALIGNED(16, tran_low_t, output_block[kNumCoeffs]);
 
     for (int i = 0; i < count_test_block; ++i) {
      // Initialize a test block with input range [-mask_, mask_].
       for (int j = 0; j < kNumCoeffs; ++j) {
        input_extreme_block[j] = rnd.Rand8() % 2 ? mask_ : -mask_;
       }
      if (i == 0) {
         for (int j = 0; j < kNumCoeffs; ++j)
          input_extreme_block[j] = mask_;
      } else if (i == 1) {
         for (int j = 0; j < kNumCoeffs; ++j)
          input_extreme_block[j] = -mask_;
      }
 
       fwd_txfm_ref(input_extreme_block, output_ref_block, pitch_, tx_type_);
      ASM_REGISTER_STATE_CHECK(RunFwdTxfm(input_extreme_block,
                                          output_block, pitch_));
 
       for (int j = 0; j < kNumCoeffs; ++j) {
         EXPECT_EQ(output_block[j], output_ref_block[j]);
        EXPECT_GE(4 * DCT_MAX_VALUE << (bit_depth_ - 8), abs(output_block[j]))
             << "Error: 16x16 FDCT has coefficient larger than 4*DCT_MAX_VALUE";
       }
     }
   }
