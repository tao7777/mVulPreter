 void RunRoundTripErrorCheck() {
 ACMRandom rnd(ACMRandom::DeterministicSeed());

     int max_error = 0;
     int total_error = 0;
     const int count_test_block = 100000;
    DECLARE_ALIGNED_ARRAY(16, int16_t, test_input_block, 64);
    DECLARE_ALIGNED_ARRAY(16, int16_t, test_temp_block, 64);
    DECLARE_ALIGNED_ARRAY(16, uint8_t, dst, 64);
    DECLARE_ALIGNED_ARRAY(16, uint8_t, src, 64);
 
     for (int i = 0; i < count_test_block; ++i) {
       for (int j = 0; j < 64; ++j) {
        src[j] = rnd.Rand8();
        dst[j] = rnd.Rand8();
        test_input_block[j] = src[j] - dst[j];
       }
 
      REGISTER_STATE_CHECK(
           RunFwdTxfm(test_input_block, test_temp_block, pitch_));
       for (int j = 0; j < 64; ++j) {
           if (test_temp_block[j] > 0) {
            test_temp_block[j] += 2;
            test_temp_block[j] /= 4;
            test_temp_block[j] *= 4;
 } else {
            test_temp_block[j] -= 2;
            test_temp_block[j] /= 4;

             test_temp_block[j] *= 4;
           }
       }
      REGISTER_STATE_CHECK(
          RunInvTxfm(test_temp_block, dst, pitch_));
 
       for (int j = 0; j < 64; ++j) {
         const int diff = dst[j] - src[j];
         const int error = diff * diff;
         if (max_error < error)
           max_error = error;
        total_error += error;

       }
     }
 
    EXPECT_GE(1, max_error)
       << "Error: 8x8 FDCT/IDCT or FHT/IHT has an individual"
       << " roundtrip error > 1";
 
    EXPECT_GE(count_test_block/5, total_error)
       << "Error: 8x8 FDCT/IDCT or FHT/IHT has average roundtrip "
       << "error > 1/5 per block";
   }
