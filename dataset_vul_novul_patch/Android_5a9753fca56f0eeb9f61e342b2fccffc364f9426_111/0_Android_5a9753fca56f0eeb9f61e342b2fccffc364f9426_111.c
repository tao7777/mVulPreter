 void VarianceTest<VarianceFunctionType>::RefTest() {
   for (int i = 0; i < 10; ++i) {
     for (int j = 0; j < block_size_; j++) {
    if (!use_high_bit_depth_) {
      src_[j] = rnd_.Rand8();
      ref_[j] = rnd_.Rand8();
#if CONFIG_VP9_HIGHBITDEPTH
    } else {
      CONVERT_TO_SHORTPTR(src_)[j] = rnd_.Rand16() && mask_;
      CONVERT_TO_SHORTPTR(ref_)[j] = rnd_.Rand16() && mask_;
#endif  // CONFIG_VP9_HIGHBITDEPTH
    }
     }
     unsigned int sse1, sse2;
     unsigned int var1;
    const int stride_coeff = 1;
    ASM_REGISTER_STATE_CHECK(
        var1 = variance_(src_, width_, ref_, width_, &sse1));
     const unsigned int var2 = variance_ref(src_, ref_, log2width_,
                                           log2height_, stride_coeff,
                                           stride_coeff, &sse2,
                                           use_high_bit_depth_, bit_depth_);
    EXPECT_EQ(sse1, sse2);
    EXPECT_EQ(var1, var2);
  }
}

template<typename VarianceFunctionType>
void VarianceTest<VarianceFunctionType>::RefStrideTest() {
  for (int i = 0; i < 10; ++i) {
    int ref_stride_coeff = i % 2;
    int src_stride_coeff = (i >> 1) % 2;
    for (int j = 0; j < block_size_; j++) {
      int ref_ind = (j / width_) * ref_stride_coeff * width_ + j % width_;
      int src_ind = (j / width_) * src_stride_coeff * width_ + j % width_;
      if (!use_high_bit_depth_) {
        src_[src_ind] = rnd_.Rand8();
        ref_[ref_ind] = rnd_.Rand8();
#if CONFIG_VP9_HIGHBITDEPTH
      } else {
        CONVERT_TO_SHORTPTR(src_)[src_ind] = rnd_.Rand16() && mask_;
        CONVERT_TO_SHORTPTR(ref_)[ref_ind] = rnd_.Rand16() && mask_;
#endif  // CONFIG_VP9_HIGHBITDEPTH
      }
    }
    unsigned int sse1, sse2;
    unsigned int var1;

    ASM_REGISTER_STATE_CHECK(
        var1 = variance_(src_, width_ * src_stride_coeff,
                         ref_, width_ * ref_stride_coeff, &sse1));
    const unsigned int var2 = variance_ref(src_, ref_, log2width_,
                                           log2height_, src_stride_coeff,
                                           ref_stride_coeff, &sse2,
                                           use_high_bit_depth_, bit_depth_);
     EXPECT_EQ(sse1, sse2);
     EXPECT_EQ(var1, var2);
   }
}
