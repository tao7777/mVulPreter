  uint8_t* output() const {
  uint8_t *output() const {
#if CONFIG_VP9_HIGHBITDEPTH
    if (UUT_->use_highbd_ == 0) {
      return output_ + BorderTop() * kOuterBlockSize + BorderLeft();
    } else {
      return CONVERT_TO_BYTEPTR(output16_ + BorderTop() * kOuterBlockSize +
                                BorderLeft());
    }
#else
     return output_ + BorderTop() * kOuterBlockSize + BorderLeft();
#endif
  }

  uint8_t *output_ref() const {
#if CONFIG_VP9_HIGHBITDEPTH
    if (UUT_->use_highbd_ == 0) {
      return output_ref_ + BorderTop() * kOuterBlockSize + BorderLeft();
    } else {
      return CONVERT_TO_BYTEPTR(output16_ref_ + BorderTop() * kOuterBlockSize +
                                BorderLeft());
    }
#else
    return output_ref_ + BorderTop() * kOuterBlockSize + BorderLeft();
#endif
  }

  uint16_t lookup(uint8_t *list, int index) const {
#if CONFIG_VP9_HIGHBITDEPTH
    if (UUT_->use_highbd_ == 0) {
      return list[index];
    } else {
      return CONVERT_TO_SHORTPTR(list)[index];
    }
#else
    return list[index];
#endif
  }

  void assign_val(uint8_t *list, int index, uint16_t val) const {
#if CONFIG_VP9_HIGHBITDEPTH
    if (UUT_->use_highbd_ == 0) {
      list[index] = (uint8_t) val;
    } else {
      CONVERT_TO_SHORTPTR(list)[index] = val;
    }
#else
    list[index] = (uint8_t) val;
#endif
  }

  void wrapper_filter_average_block2d_8_c(const uint8_t *src_ptr,
                                          const unsigned int src_stride,
                                          const int16_t *HFilter,
                                          const int16_t *VFilter,
                                          uint8_t *dst_ptr,
                                          unsigned int dst_stride,
                                          unsigned int output_width,
                                          unsigned int output_height) {
#if CONFIG_VP9_HIGHBITDEPTH
    if (UUT_->use_highbd_ == 0) {
      filter_average_block2d_8_c(src_ptr, src_stride, HFilter, VFilter,
                                 dst_ptr, dst_stride, output_width,
                                 output_height);
    } else {
      highbd_filter_average_block2d_8_c(CONVERT_TO_SHORTPTR(src_ptr),
                                        src_stride, HFilter, VFilter,
                                        CONVERT_TO_SHORTPTR(dst_ptr),
                                        dst_stride, output_width, output_height,
                                        UUT_->use_highbd_);
    }
#else
    filter_average_block2d_8_c(src_ptr, src_stride, HFilter, VFilter,
                               dst_ptr, dst_stride, output_width,
                               output_height);
#endif
  }

  void wrapper_filter_block2d_8_c(const uint8_t *src_ptr,
                                  const unsigned int src_stride,
                                  const int16_t *HFilter,
                                  const int16_t *VFilter,
                                  uint8_t *dst_ptr,
                                  unsigned int dst_stride,
                                  unsigned int output_width,
                                  unsigned int output_height) {
#if CONFIG_VP9_HIGHBITDEPTH
    if (UUT_->use_highbd_ == 0) {
      filter_block2d_8_c(src_ptr, src_stride, HFilter, VFilter,
                         dst_ptr, dst_stride, output_width, output_height);
    } else {
      highbd_filter_block2d_8_c(CONVERT_TO_SHORTPTR(src_ptr), src_stride,
                                HFilter, VFilter,
                                CONVERT_TO_SHORTPTR(dst_ptr), dst_stride,
                                output_width, output_height, UUT_->use_highbd_);
    }
#else
    filter_block2d_8_c(src_ptr, src_stride, HFilter, VFilter,
                       dst_ptr, dst_stride, output_width, output_height);
#endif
   }
