 void SubpelVarianceTest<SubpelVarianceFunctionType>::RefTest() {
  for (int x = 0; x < 16; ++x) {
    for (int y = 0; y < 16; ++y) {
      for (int j = 0; j < block_size_; j++) {
        src_[j] = rnd.Rand8();
      }
      for (int j = 0; j < block_size_ + width_ + height_ + 1; j++) {
        ref_[j] = rnd.Rand8();
       }
       unsigned int sse1, sse2;
       unsigned int var1;
      REGISTER_STATE_CHECK(var1 = subpel_variance_(ref_, width_ + 1, x, y,
                                                   src_, width_, &sse1));
      const unsigned int var2 = subpel_variance_ref(ref_, src_, log2width_,
                                                    log2height_, x, y, &sse2);
       EXPECT_EQ(sse1, sse2) << "at position " << x << ", " << y;
       EXPECT_EQ(var1, var2) << "at position " << x << ", " << y;
     }
   }
 }
