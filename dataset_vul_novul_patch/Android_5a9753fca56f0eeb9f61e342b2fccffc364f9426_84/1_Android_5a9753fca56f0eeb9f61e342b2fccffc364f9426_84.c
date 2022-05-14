 void RunExtremalCheck() {

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
        src[j] = rnd.Rand8() % 2 ? 255 : 0;
        dst[j] = src[j] > 0 ? 0 : 255;
        test_input_block[j] = src[j] - dst[j];
       }
 
      REGISTER_STATE_CHECK(
           RunFwdTxfm(test_input_block, test_temp_block, pitch_));
      REGISTER_STATE_CHECK(
          RunInvTxfm(test_temp_block, dst, pitch_));
 
       for (int j = 0; j < 64; ++j) {
         const int diff = dst[j] - src[j];
         const int error = diff * diff;
         if (max_error < error)
           max_error = error;
         total_error += error;
       }
 
      EXPECT_GE(1, max_error)
           << "Error: Extremal 8x8 FDCT/IDCT or FHT/IHT has"
           << "an individual roundtrip error > 1";
 
      EXPECT_GE(count_test_block/5, total_error)
           << "Error: Extremal 8x8 FDCT/IDCT or FHT/IHT has average"
           << " roundtrip error > 1/5 per block";
     }
   }
