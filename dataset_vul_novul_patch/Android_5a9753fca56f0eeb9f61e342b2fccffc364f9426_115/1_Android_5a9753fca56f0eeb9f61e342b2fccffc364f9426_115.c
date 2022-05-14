   virtual void SetUp() {
    const tuple<int, int, SubpelVarianceFunctionType>& params =
        this->GetParam();
     log2width_  = get<0>(params);
     width_ = 1 << log2width_;
     log2height_ = get<1>(params);
     height_ = 1 << log2height_;
    subpel_variance_ = get<2>(params);
 
     rnd(ACMRandom::DeterministicSeed());
     block_size_ = width_ * height_;
     src_ = reinterpret_cast<uint8_t *>(vpx_memalign(16, block_size_));
    sec_ = reinterpret_cast<uint8_t *>(vpx_memalign(16, block_size_));
    ref_ = new uint8_t[block_size_ + width_ + height_ + 1];
     ASSERT_TRUE(src_ != NULL);
    ASSERT_TRUE(sec_ != NULL);
     ASSERT_TRUE(ref_ != NULL);
   }
