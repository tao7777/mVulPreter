 void RunExtremalCheck() {

     ACMRandom rnd(ACMRandom::DeterministicSeed());
     int max_error = 0;
     int total_error = 0;
    int total_coeff_error = 0;
     const int count_test_block = 100000;
    DECLARE_ALIGNED(16, int16_t, test_input_block[64]);
    DECLARE_ALIGNED(16, tran_low_t, test_temp_block[64]);
    DECLARE_ALIGNED(16, tran_low_t, ref_temp_block[64]);
    DECLARE_ALIGNED(16, uint8_t, dst[64]);
    DECLARE_ALIGNED(16, uint8_t, src[64]);
#if CONFIG_VP9_HIGHBITDEPTH
    DECLARE_ALIGNED(16, uint16_t, dst16[64]);
    DECLARE_ALIGNED(16, uint16_t, src16[64]);
#endif
 
     for (int i = 0; i < count_test_block; ++i) {
      // Initialize a test block with input range [-mask_, mask_].
       for (int j = 0; j < 64; ++j) {
        if (bit_depth_ == VPX_BITS_8) {
          if (i == 0) {
            src[j] = 255;
            dst[j] = 0;
          } else if (i == 1) {
            src[j] = 0;
            dst[j] = 255;
          } else {
            src[j] = rnd.Rand8() % 2 ? 255 : 0;
            dst[j] = rnd.Rand8() % 2 ? 255 : 0;
          }
          test_input_block[j] = src[j] - dst[j];
#if CONFIG_VP9_HIGHBITDEPTH
        } else {
          if (i == 0) {
            src16[j] = mask_;
            dst16[j] = 0;
          } else if (i == 1) {
            src16[j] = 0;
            dst16[j] = mask_;
          } else {
            src16[j] = rnd.Rand8() % 2 ? mask_ : 0;
            dst16[j] = rnd.Rand8() % 2 ? mask_ : 0;
          }
          test_input_block[j] = src16[j] - dst16[j];
#endif
        }
       }
 
      ASM_REGISTER_STATE_CHECK(
           RunFwdTxfm(test_input_block, test_temp_block, pitch_));
      ASM_REGISTER_STATE_CHECK(
          fwd_txfm_ref(test_input_block, ref_temp_block, pitch_, tx_type_));
      if (bit_depth_ == VPX_BITS_8) {
        ASM_REGISTER_STATE_CHECK(
            RunInvTxfm(test_temp_block, dst, pitch_));
#if CONFIG_VP9_HIGHBITDEPTH
      } else {
        ASM_REGISTER_STATE_CHECK(
            RunInvTxfm(test_temp_block, CONVERT_TO_BYTEPTR(dst16), pitch_));
#endif
      }
 
       for (int j = 0; j < 64; ++j) {
#if CONFIG_VP9_HIGHBITDEPTH
        const int diff =
            bit_depth_ == VPX_BITS_8 ? dst[j] - src[j] : dst16[j] - src16[j];
#else
         const int diff = dst[j] - src[j];
#endif
         const int error = diff * diff;
         if (max_error < error)
           max_error = error;
         total_error += error;

        const int coeff_diff = test_temp_block[j] - ref_temp_block[j];
        total_coeff_error += abs(coeff_diff);
       }
 
      EXPECT_GE(1 << 2 * (bit_depth_ - 8), max_error)
           << "Error: Extremal 8x8 FDCT/IDCT or FHT/IHT has"
           << "an individual roundtrip error > 1";
 
      EXPECT_GE((count_test_block << 2 * (bit_depth_ - 8))/5, total_error)
           << "Error: Extremal 8x8 FDCT/IDCT or FHT/IHT has average"
           << " roundtrip error > 1/5 per block";

      EXPECT_EQ(0, total_coeff_error)
          << "Error: Extremal 8x8 FDCT/FHT has"
          << "overflow issues in the intermediate steps > 1";
     }
   }
