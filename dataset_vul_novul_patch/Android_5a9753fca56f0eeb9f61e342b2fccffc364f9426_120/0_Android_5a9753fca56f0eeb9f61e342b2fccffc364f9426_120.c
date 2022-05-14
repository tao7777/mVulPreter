static unsigned int subpel_variance_ref(const uint8_t *ref, const uint8_t *src,
static unsigned int mb_ss_ref(const int16_t *src) {
  unsigned int res = 0;
  for (int i = 0; i < 256; ++i) {
    res += src[i] * src[i];
  }
  return res;
}

static uint32_t variance_ref(const uint8_t *src, const uint8_t *ref,
                             int l2w, int l2h, int src_stride_coeff,
                             int ref_stride_coeff, uint32_t *sse_ptr,
                             bool use_high_bit_depth_,
                             vpx_bit_depth_t bit_depth) {
  int64_t se = 0;
  uint64_t sse = 0;
  const int w = 1 << l2w;
  const int h = 1 << l2h;
   for (int y = 0; y < h; y++) {
     for (int x = 0; x < w; x++) {
      int diff;
      if (!use_high_bit_depth_) {
        diff = ref[w * y * ref_stride_coeff + x] -
               src[w * y * src_stride_coeff + x];
        se += diff;
        sse += diff * diff;
#if CONFIG_VP9_HIGHBITDEPTH
      } else {
        diff = CONVERT_TO_SHORTPTR(ref)[w * y * ref_stride_coeff + x] -
               CONVERT_TO_SHORTPTR(src)[w * y * src_stride_coeff + x];
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

/* The subpel reference functions differ from the codec version in one aspect:
 * they calculate the bilinear factors directly instead of using a lookup table
 * and therefore upshift xoff and yoff by 1. Only every other calculated value
 * is used so the codec version shrinks the table to save space and maintain
 * compatibility with vp8.
 */
static uint32_t subpel_variance_ref(const uint8_t *ref, const uint8_t *src,
                                    int l2w, int l2h, int xoff, int yoff,
                                    uint32_t *sse_ptr,
                                    bool use_high_bit_depth_,
                                    vpx_bit_depth_t bit_depth) {
  int64_t se = 0;
  uint64_t sse = 0;
  const int w = 1 << l2w;
  const int h = 1 << l2h;

  xoff <<= 1;
  yoff <<= 1;

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      // Bilinear interpolation at a 16th pel step.
      if (!use_high_bit_depth_) {
        const int a1 = ref[(w + 1) * (y + 0) + x + 0];
        const int a2 = ref[(w + 1) * (y + 0) + x + 1];
        const int b1 = ref[(w + 1) * (y + 1) + x + 0];
        const int b2 = ref[(w + 1) * (y + 1) + x + 1];
        const int a = a1 + (((a2 - a1) * xoff + 8) >> 4);
        const int b = b1 + (((b2 - b1) * xoff + 8) >> 4);
        const int r = a + (((b - a) * yoff + 8) >> 4);
        const int diff = r - src[w * y + x];
        se += diff;
        sse += diff * diff;
#if CONFIG_VP9_HIGHBITDEPTH
      } else {
        uint16_t *ref16 = CONVERT_TO_SHORTPTR(ref);
        uint16_t *src16 = CONVERT_TO_SHORTPTR(src);
        const int a1 = ref16[(w + 1) * (y + 0) + x + 0];
        const int a2 = ref16[(w + 1) * (y + 0) + x + 1];
        const int b1 = ref16[(w + 1) * (y + 1) + x + 0];
        const int b2 = ref16[(w + 1) * (y + 1) + x + 1];
        const int a = a1 + (((a2 - a1) * xoff + 8) >> 4);
        const int b = b1 + (((b2 - b1) * xoff + 8) >> 4);
        const int r = a + (((b - a) * yoff + 8) >> 4);
        const int diff = r - src16[w * y + x];
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

class SumOfSquaresTest : public ::testing::TestWithParam<SumOfSquaresFunction> {
 public:
  SumOfSquaresTest() : func_(GetParam()) {}

  virtual ~SumOfSquaresTest() {
    libvpx_test::ClearSystemState();
  }

 protected:
  void ConstTest();
  void RefTest();

  SumOfSquaresFunction func_;
  ACMRandom rnd_;
};

void SumOfSquaresTest::ConstTest() {
  int16_t mem[256];
  unsigned int res;
  for (int v = 0; v < 256; ++v) {
    for (int i = 0; i < 256; ++i) {
      mem[i] = v;
    }
    ASM_REGISTER_STATE_CHECK(res = func_(mem));
    EXPECT_EQ(256u * (v * v), res);
  }
}

void SumOfSquaresTest::RefTest() {
  int16_t mem[256];
  for (int i = 0; i < 100; ++i) {
    for (int j = 0; j < 256; ++j) {
      mem[j] = rnd_.Rand8() - rnd_.Rand8();
    }

    const unsigned int expected = mb_ss_ref(mem);
    unsigned int res;
    ASM_REGISTER_STATE_CHECK(res = func_(mem));
    EXPECT_EQ(expected, res);
  }
 }
