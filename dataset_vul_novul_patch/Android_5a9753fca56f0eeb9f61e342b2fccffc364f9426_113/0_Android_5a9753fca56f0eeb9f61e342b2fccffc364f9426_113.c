void SubpelVarianceTest<vp9_subp_avg_variance_fn_t>::RefTest() {
void SubpelVarianceTest<SubpixAvgVarMxNFunc>::RefTest() {
  for (int x = 0; x < 8; ++x) {
    for (int y = 0; y < 8; ++y) {
      if (!use_high_bit_depth_) {
        for (int j = 0; j < block_size_; j++) {
          src_[j] = rnd_.Rand8();
          sec_[j] = rnd_.Rand8();
        }
        for (int j = 0; j < block_size_ + width_ + height_ + 1; j++) {
          ref_[j] = rnd_.Rand8();
        }
#if CONFIG_VP9_HIGHBITDEPTH
      } else {
        for (int j = 0; j < block_size_; j++) {
          CONVERT_TO_SHORTPTR(src_)[j] = rnd_.Rand16() & mask_;
          CONVERT_TO_SHORTPTR(sec_)[j] = rnd_.Rand16() & mask_;
        }
        for (int j = 0; j < block_size_ + width_ + height_ + 1; j++) {
          CONVERT_TO_SHORTPTR(ref_)[j] = rnd_.Rand16() & mask_;
        }
#endif  // CONFIG_VP9_HIGHBITDEPTH
       }
       unsigned int sse1, sse2;
       unsigned int var1;
      ASM_REGISTER_STATE_CHECK(
          var1 = subpel_variance_(ref_, width_ + 1, x, y,
                                  src_, width_, &sse1, sec_));
       const unsigned int var2 = subpel_avg_variance_ref(ref_, src_, sec_,
                                                         log2width_, log2height_,
                                                        x, y, &sse2,
                                                        use_high_bit_depth_,
                                                        bit_depth_);
       EXPECT_EQ(sse1, sse2) << "at position " << x << ", " << y;
       EXPECT_EQ(var1, var2) << "at position " << x << ", " << y;
     }
   }
 }
