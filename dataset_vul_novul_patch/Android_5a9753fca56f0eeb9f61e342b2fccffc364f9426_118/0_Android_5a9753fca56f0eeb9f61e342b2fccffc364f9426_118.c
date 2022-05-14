 void VarianceTest<VarianceFunctionType>::ZeroTest() {
   for (int i = 0; i <= 255; ++i) {
    if (!use_high_bit_depth_) {
      memset(src_, i, block_size_);
#if CONFIG_VP9_HIGHBITDEPTH
    } else {
      vpx_memset16(CONVERT_TO_SHORTPTR(src_), i << (bit_depth_ - 8),
                   block_size_);
#endif  // CONFIG_VP9_HIGHBITDEPTH
    }
     for (int j = 0; j <= 255; ++j) {
      if (!use_high_bit_depth_) {
        memset(ref_, j, block_size_);
#if CONFIG_VP9_HIGHBITDEPTH
      } else {
        vpx_memset16(CONVERT_TO_SHORTPTR(ref_), j  << (bit_depth_ - 8),
                     block_size_);
#endif  // CONFIG_VP9_HIGHBITDEPTH
      }
       unsigned int sse;
       unsigned int var;
      ASM_REGISTER_STATE_CHECK(
          var = variance_(src_, width_, ref_, width_, &sse));
      EXPECT_EQ(0u, var) << "src values: " << i << " ref values: " << j;
     }
   }
 }
