  unsigned int ReferenceSAD(unsigned int max_sad, int block_idx = 0) {
  unsigned int ReferenceSAD(int block_idx) {
     unsigned int sad = 0;
      const uint8_t *const reference8 = GetReference(block_idx);
      const uint8_t *const source8 = source_data_;
#if CONFIG_VP9_HIGHBITDEPTH
      const uint16_t *const reference16 =
          CONVERT_TO_SHORTPTR(GetReference(block_idx));
      const uint16_t *const source16 = CONVERT_TO_SHORTPTR(source_data_);
#endif  // CONFIG_VP9_HIGHBITDEPTH
     for (int h = 0; h < height_; ++h) {
       for (int w = 0; w < width_; ++w) {
        if (!use_high_bit_depth_) {
          sad += abs(source8[h * source_stride_ + w] -
                     reference8[h * reference_stride_ + w]);
#if CONFIG_VP9_HIGHBITDEPTH
        } else {
          sad += abs(source16[h * source_stride_ + w] -
                     reference16[h * reference_stride_ + w]);
#endif  // CONFIG_VP9_HIGHBITDEPTH
        }
       }
     }
     return sad;
   }
