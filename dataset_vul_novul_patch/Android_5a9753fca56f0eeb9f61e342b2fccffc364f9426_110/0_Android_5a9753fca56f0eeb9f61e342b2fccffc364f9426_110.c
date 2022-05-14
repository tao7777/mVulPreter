 void VarianceTest<VarianceFunctionType>::OneQuarterTest() {
   const int half = block_size_ / 2;
  if (!use_high_bit_depth_) {
    memset(src_, 255, block_size_);
    memset(ref_, 255, half);
    memset(ref_ + half, 0, half);
#if CONFIG_VP9_HIGHBITDEPTH
  } else {
    vpx_memset16(CONVERT_TO_SHORTPTR(src_), 255 << (bit_depth_ - 8),
                 block_size_);
    vpx_memset16(CONVERT_TO_SHORTPTR(ref_), 255 << (bit_depth_ - 8), half);
    vpx_memset16(CONVERT_TO_SHORTPTR(ref_) + half, 0, half);
#endif  // CONFIG_VP9_HIGHBITDEPTH
  }
   unsigned int sse;
   unsigned int var;
  ASM_REGISTER_STATE_CHECK(var = variance_(src_, width_, ref_, width_, &sse));
   const unsigned int expected = block_size_ * 255 * 255 / 4;
   EXPECT_EQ(expected, var);
 }
