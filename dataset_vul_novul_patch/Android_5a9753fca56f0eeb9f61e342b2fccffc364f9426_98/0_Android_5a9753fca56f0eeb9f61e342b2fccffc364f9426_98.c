  virtual uint8_t* GetReference(int block_idx) {
  virtual uint8_t *GetReference(int block_idx) {
#if CONFIG_VP9_HIGHBITDEPTH
    if (use_high_bit_depth_)
      return CONVERT_TO_BYTEPTR(CONVERT_TO_SHORTPTR(reference_data_) +
                                block_idx * kDataBlockSize);
#endif  // CONFIG_VP9_HIGHBITDEPTH
     return reference_data_ + block_idx * kDataBlockSize;
   }
