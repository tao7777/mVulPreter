 void VarianceTest<VarianceFunctionType>::OneQuarterTest() {
  memset(src_, 255, block_size_);
   const int half = block_size_ / 2;
  memset(ref_, 255, half);
  memset(ref_ + half, 0, half);
   unsigned int sse;
   unsigned int var;
  REGISTER_STATE_CHECK(var = variance_(src_, width_, ref_, width_, &sse));
   const unsigned int expected = block_size_ * 255 * 255 / 4;
   EXPECT_EQ(expected, var);
 }
