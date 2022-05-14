   virtual void SetUp() {
    const tuple<int, int, VarianceFunctionType, int>& params = this->GetParam();
     log2width_  = get<0>(params);
     width_ = 1 << log2width_;
     log2height_ = get<1>(params);
     height_ = 1 << log2height_;
     variance_ = get<2>(params);
    if (get<3>(params)) {
      bit_depth_ = static_cast<vpx_bit_depth_t>(get<3>(params));
      use_high_bit_depth_ = true;
    } else {
      bit_depth_ = VPX_BITS_8;
      use_high_bit_depth_ = false;
    }
    mask_ = (1 << bit_depth_) - 1;
 
    rnd_.Reset(ACMRandom::DeterministicSeed());
     block_size_ = width_ * height_;
    if (!use_high_bit_depth_) {
      src_ = reinterpret_cast<uint8_t *>(vpx_memalign(16, block_size_ * 2));
      ref_ = new uint8_t[block_size_ * 2];
#if CONFIG_VP9_HIGHBITDEPTH
    } else {
      src_ = CONVERT_TO_BYTEPTR(reinterpret_cast<uint16_t *>(
          vpx_memalign(16, block_size_ * 2 * sizeof(uint16_t))));
      ref_ = CONVERT_TO_BYTEPTR(new uint16_t[block_size_ * 2]);
#endif  // CONFIG_VP9_HIGHBITDEPTH
    }
     ASSERT_TRUE(src_ != NULL);
     ASSERT_TRUE(ref_ != NULL);
   }
