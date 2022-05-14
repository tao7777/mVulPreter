   void RunSignBiasCheck() {
     ACMRandom rnd(ACMRandom::DeterministicSeed());
    DECLARE_ALIGNED_ARRAY(16, int16_t, test_input_block, 64);
    DECLARE_ALIGNED_ARRAY(16, int16_t, test_output_block, 64);
     int count_sign_block[64][2];
     const int count_test_block = 100000;
 
    memset(count_sign_block, 0, sizeof(count_sign_block));


     for (int i = 0; i < count_test_block; ++i) {
       for (int j = 0; j < 64; ++j)
        test_input_block[j] = rnd.Rand8() - rnd.Rand8();
      REGISTER_STATE_CHECK(
           RunFwdTxfm(test_input_block, test_output_block, pitch_));
 
       for (int j = 0; j < 64; ++j) {
 if (test_output_block[j] < 0)
 ++count_sign_block[j][0];
 else if (test_output_block[j] > 0)
 ++count_sign_block[j][1];
 }
 }

 
     for (int j = 0; j < 64; ++j) {
       const int diff = abs(count_sign_block[j][0] - count_sign_block[j][1]);
      const int max_diff = 1125;
      EXPECT_LT(diff, max_diff)
           << "Error: 8x8 FDCT/FHT has a sign bias > "
           << 1. * max_diff / count_test_block * 100 << "%"
           << " for input range [-255, 255] at index " << j
 << " count0: " << count_sign_block[j][0]
 << " count1: " << count_sign_block[j][1]
 << " diff: " << diff;
 }


     memset(count_sign_block, 0, sizeof(count_sign_block));
 
     for (int i = 0; i < count_test_block; ++i) {
       for (int j = 0; j < 64; ++j)
        test_input_block[j] = (rnd.Rand8() >> 4) - (rnd.Rand8() >> 4);
      REGISTER_STATE_CHECK(
           RunFwdTxfm(test_input_block, test_output_block, pitch_));
 
       for (int j = 0; j < 64; ++j) {
 if (test_output_block[j] < 0)
 ++count_sign_block[j][0];
 else if (test_output_block[j] > 0)
 ++count_sign_block[j][1];
 }
 }

 
     for (int j = 0; j < 64; ++j) {
       const int diff = abs(count_sign_block[j][0] - count_sign_block[j][1]);
      const int max_diff = 10000;
      EXPECT_LT(diff, max_diff)
          << "Error: 4x4 FDCT/FHT has a sign bias > "
           << 1. * max_diff / count_test_block * 100 << "%"
           << " for input range [-15, 15] at index " << j
           << " count0: " << count_sign_block[j][0]
 << " count1: " << count_sign_block[j][1]
 << " diff: " << diff;
 }
 }
