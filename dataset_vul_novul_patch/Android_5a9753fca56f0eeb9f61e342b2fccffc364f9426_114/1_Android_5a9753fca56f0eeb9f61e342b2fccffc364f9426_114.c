   virtual void SetUp() {
    const tuple<int, int, VarianceFunctionType>& params = this->GetParam();
     log2width_  = get<0>(params);
     width_ = 1 << log2width_;
     log2height_ = get<1>(params);
     height_ = 1 << log2height_;
     variance_ = get<2>(params);
 
    rnd(ACMRandom::DeterministicSeed());
     block_size_ = width_ * height_;
    src_ = new uint8_t[block_size_];
    ref_ = new uint8_t[block_size_];
     ASSERT_TRUE(src_ != NULL);
     ASSERT_TRUE(ref_ != NULL);
   }
