  void FillConstant(uint8_t *data, int stride, uint8_t fill_constant) {
  // Sum of Absolute Differences Average. Given two blocks, and a prediction
  // calculate the absolute difference between one pixel and average of the
  // corresponding and predicted pixels; accumulate.
  unsigned int ReferenceSADavg(int block_idx) {
    unsigned int sad = 0;
    const uint8_t *const reference8 = GetReference(block_idx);
    const uint8_t *const source8 = source_data_;
    const uint8_t *const second_pred8 = second_pred_;
#if CONFIG_VP9_HIGHBITDEPTH
    const uint16_t *const reference16 =
        CONVERT_TO_SHORTPTR(GetReference(block_idx));
    const uint16_t *const source16 = CONVERT_TO_SHORTPTR(source_data_);
    const uint16_t *const second_pred16 = CONVERT_TO_SHORTPTR(second_pred_);
#endif  // CONFIG_VP9_HIGHBITDEPTH
     for (int h = 0; h < height_; ++h) {
       for (int w = 0; w < width_; ++w) {
        if (!use_high_bit_depth_) {
          const int tmp = second_pred8[h * width_ + w] +
              reference8[h * reference_stride_ + w];
          const uint8_t comp_pred = ROUND_POWER_OF_TWO(tmp, 1);
          sad += abs(source8[h * source_stride_ + w] - comp_pred);
#if CONFIG_VP9_HIGHBITDEPTH
        } else {
          const int tmp = second_pred16[h * width_ + w] +
              reference16[h * reference_stride_ + w];
          const uint16_t comp_pred = ROUND_POWER_OF_TWO(tmp, 1);
          sad += abs(source16[h * source_stride_ + w] - comp_pred);
#endif  // CONFIG_VP9_HIGHBITDEPTH
        }
      }
    }
    return sad;
  }

  void FillConstant(uint8_t *data, int stride, uint16_t fill_constant) {
    uint8_t *data8 = data;
#if CONFIG_VP9_HIGHBITDEPTH
    uint16_t *data16 = CONVERT_TO_SHORTPTR(data);
#endif  // CONFIG_VP9_HIGHBITDEPTH
    for (int h = 0; h < height_; ++h) {
      for (int w = 0; w < width_; ++w) {
        if (!use_high_bit_depth_) {
          data8[h * stride + w] = static_cast<uint8_t>(fill_constant);
#if CONFIG_VP9_HIGHBITDEPTH
        } else {
          data16[h * stride + w] = fill_constant;
#endif  // CONFIG_VP9_HIGHBITDEPTH
        }
       }
     }
   }
