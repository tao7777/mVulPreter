   void FillRandom(uint8_t *data, int stride) {
    uint8_t *data8 = data;
#if CONFIG_VP9_HIGHBITDEPTH
    uint16_t *data16 = CONVERT_TO_SHORTPTR(data);
#endif  // CONFIG_VP9_HIGHBITDEPTH
     for (int h = 0; h < height_; ++h) {
       for (int w = 0; w < width_; ++w) {
        if (!use_high_bit_depth_) {
          data8[h * stride + w] = rnd_.Rand8();
#if CONFIG_VP9_HIGHBITDEPTH
        } else {
          data16[h * stride + w] = rnd_.Rand16() & mask_;
#endif  // CONFIG_VP9_HIGHBITDEPTH
        }
       }
     }
   }
