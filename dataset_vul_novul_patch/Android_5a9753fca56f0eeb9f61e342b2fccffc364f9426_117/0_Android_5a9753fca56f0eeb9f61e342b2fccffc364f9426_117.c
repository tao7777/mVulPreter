   virtual void TearDown() {
     vpx_free(src_);
     delete[] ref_;
    libvpx_test::ClearSystemState();
  }

 protected:
  void RefTest_mse();
  void RefTest_sse();
  void MaxTest_mse();
  void MaxTest_sse();

  ACMRandom rnd;
  uint8_t* src_;
  uint8_t* ref_;
  int width_, log2width_;
  int height_, log2height_;
  int block_size_;
  MseFunctionType mse_;
};

template<typename MseFunctionType>
void MseTest<MseFunctionType>::RefTest_mse() {
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < block_size_; j++) {
      src_[j] = rnd.Rand8();
      ref_[j] = rnd.Rand8();
    }
    unsigned int sse1, sse2;
    const int stride_coeff = 1;
    ASM_REGISTER_STATE_CHECK(mse_(src_, width_, ref_, width_, &sse1));
    variance_ref(src_, ref_, log2width_, log2height_, stride_coeff,
                 stride_coeff, &sse2, false, VPX_BITS_8);
    EXPECT_EQ(sse1, sse2);
  }
}

template<typename MseFunctionType>
void MseTest<MseFunctionType>::RefTest_sse() {
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < block_size_; j++) {
      src_[j] = rnd.Rand8();
      ref_[j] = rnd.Rand8();
    }
    unsigned int sse2;
    unsigned int var1;
    const int stride_coeff = 1;
    ASM_REGISTER_STATE_CHECK(var1 = mse_(src_, width_, ref_, width_));
    variance_ref(src_, ref_, log2width_, log2height_, stride_coeff,
                 stride_coeff, &sse2, false, VPX_BITS_8);
    EXPECT_EQ(var1, sse2);
  }
}

template<typename MseFunctionType>
void MseTest<MseFunctionType>::MaxTest_mse() {
  memset(src_, 255, block_size_);
  memset(ref_, 0, block_size_);
  unsigned int sse;
  ASM_REGISTER_STATE_CHECK(mse_(src_, width_, ref_, width_, &sse));
  const unsigned int expected = block_size_ * 255 * 255;
  EXPECT_EQ(expected, sse);
}

template<typename MseFunctionType>
void MseTest<MseFunctionType>::MaxTest_sse() {
  memset(src_, 255, block_size_);
  memset(ref_, 0, block_size_);
  unsigned int var;
  ASM_REGISTER_STATE_CHECK(var = mse_(src_, width_, ref_, width_));
  const unsigned int expected = block_size_ * 255 * 255;
  EXPECT_EQ(expected, var);
}

static uint32_t subpel_avg_variance_ref(const uint8_t *ref,
                                        const uint8_t *src,
                                        const uint8_t *second_pred,
                                        int l2w, int l2h,
                                        int xoff, int yoff,
                                        uint32_t *sse_ptr,
                                        bool use_high_bit_depth,
                                        vpx_bit_depth_t bit_depth) {
  int64_t se = 0;
  uint64_t sse = 0;
  const int w = 1 << l2w;
  const int h = 1 << l2h;

  xoff <<= 1;
  yoff <<= 1;

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      // bilinear interpolation at a 16th pel step
      if (!use_high_bit_depth) {
        const int a1 = ref[(w + 1) * (y + 0) + x + 0];
        const int a2 = ref[(w + 1) * (y + 0) + x + 1];
        const int b1 = ref[(w + 1) * (y + 1) + x + 0];
        const int b2 = ref[(w + 1) * (y + 1) + x + 1];
        const int a = a1 + (((a2 - a1) * xoff + 8) >> 4);
        const int b = b1 + (((b2 - b1) * xoff + 8) >> 4);
        const int r = a + (((b - a) * yoff + 8) >> 4);
        const int diff = ((r + second_pred[w * y + x] + 1) >> 1) - src[w * y + x];
        se += diff;
        sse += diff * diff;
#if CONFIG_VP9_HIGHBITDEPTH
      } else {
        uint16_t *ref16 = CONVERT_TO_SHORTPTR(ref);
        uint16_t *src16 = CONVERT_TO_SHORTPTR(src);
        uint16_t *sec16   = CONVERT_TO_SHORTPTR(second_pred);
        const int a1 = ref16[(w + 1) * (y + 0) + x + 0];
        const int a2 = ref16[(w + 1) * (y + 0) + x + 1];
        const int b1 = ref16[(w + 1) * (y + 1) + x + 0];
        const int b2 = ref16[(w + 1) * (y + 1) + x + 1];
        const int a = a1 + (((a2 - a1) * xoff + 8) >> 4);
        const int b = b1 + (((b2 - b1) * xoff + 8) >> 4);
        const int r = a + (((b - a) * yoff + 8) >> 4);
        const int diff = ((r + sec16[w * y + x] + 1) >> 1) - src16[w * y + x];
        se += diff;
        sse += diff * diff;
#endif  // CONFIG_VP9_HIGHBITDEPTH
      }
    }
  }
  RoundHighBitDepth(bit_depth, &se, &sse);
  *sse_ptr = static_cast<uint32_t>(sse);
  return static_cast<uint32_t>(sse -
                               ((static_cast<int64_t>(se) * se) >>
                                (l2w + l2h)));
}

template<typename SubpelVarianceFunctionType>
class SubpelVarianceTest
    : public ::testing::TestWithParam<tuple<int, int,
                                            SubpelVarianceFunctionType, int> > {
 public:
  virtual void SetUp() {
    const tuple<int, int, SubpelVarianceFunctionType, int>& params =
        this->GetParam();
    log2width_  = get<0>(params);
    width_ = 1 << log2width_;
    log2height_ = get<1>(params);
    height_ = 1 << log2height_;
    subpel_variance_ = get<2>(params);
    if (get<3>(params)) {
      bit_depth_ = (vpx_bit_depth_t) get<3>(params);
      use_high_bit_depth_ = true;
    } else {
      bit_depth_ = VPX_BITS_8;
      use_high_bit_depth_ = false;
    }
    mask_ = (1 << bit_depth_)-1;

    rnd_.Reset(ACMRandom::DeterministicSeed());
    block_size_ = width_ * height_;
    if (!use_high_bit_depth_) {
      src_ = reinterpret_cast<uint8_t *>(vpx_memalign(16, block_size_));
      sec_ = reinterpret_cast<uint8_t *>(vpx_memalign(16, block_size_));
      ref_ = new uint8_t[block_size_ + width_ + height_ + 1];
#if CONFIG_VP9_HIGHBITDEPTH
    } else {
      src_ = CONVERT_TO_BYTEPTR(
          reinterpret_cast<uint16_t *>(
              vpx_memalign(16, block_size_*sizeof(uint16_t))));
      sec_ = CONVERT_TO_BYTEPTR(
          reinterpret_cast<uint16_t *>(
              vpx_memalign(16, block_size_*sizeof(uint16_t))));
      ref_ = CONVERT_TO_BYTEPTR(
          new uint16_t[block_size_ + width_ + height_ + 1]);
#endif  // CONFIG_VP9_HIGHBITDEPTH
    }
    ASSERT_TRUE(src_ != NULL);
    ASSERT_TRUE(sec_ != NULL);
    ASSERT_TRUE(ref_ != NULL);
  }

  virtual void TearDown() {
    if (!use_high_bit_depth_) {
      vpx_free(src_);
      delete[] ref_;
      vpx_free(sec_);
#if CONFIG_VP9_HIGHBITDEPTH
    } else {
      vpx_free(CONVERT_TO_SHORTPTR(src_));
      delete[] CONVERT_TO_SHORTPTR(ref_);
      vpx_free(CONVERT_TO_SHORTPTR(sec_));
#endif  // CONFIG_VP9_HIGHBITDEPTH
    }
     libvpx_test::ClearSystemState();
   }
