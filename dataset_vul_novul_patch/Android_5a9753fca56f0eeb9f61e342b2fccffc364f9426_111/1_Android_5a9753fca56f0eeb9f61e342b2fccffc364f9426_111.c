 void VarianceTest<VarianceFunctionType>::RefTest() {
   for (int i = 0; i < 10; ++i) {
     for (int j = 0; j < block_size_; j++) {
      src_[j] = rnd.Rand8();
      ref_[j] = rnd.Rand8();
     }
     unsigned int sse1, sse2;
     unsigned int var1;
    REGISTER_STATE_CHECK(var1 = variance_(src_, width_, ref_, width_, &sse1));
     const unsigned int var2 = variance_ref(src_, ref_, log2width_,
                                           log2height_, &sse2);
     EXPECT_EQ(sse1, sse2);
     EXPECT_EQ(var1, var2);
   }
}
