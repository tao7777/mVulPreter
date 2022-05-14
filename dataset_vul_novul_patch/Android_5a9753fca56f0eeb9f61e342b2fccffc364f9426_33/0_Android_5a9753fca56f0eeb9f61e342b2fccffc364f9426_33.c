void filter_average_block2d_8_c(const uint8_t *src_ptr,
 const unsigned int src_stride,
 const int16_t *HFilter,
 const int16_t *VFilter,
 uint8_t *dst_ptr,

                                 unsigned int dst_stride,
                                 unsigned int output_width,
                                 unsigned int output_height) {
  uint8_t tmp[kMaxDimension * kMaxDimension];
 
  assert(output_width <= kMaxDimension);
  assert(output_height <= kMaxDimension);
   filter_block2d_8_c(src_ptr, src_stride, HFilter, VFilter, tmp, 64,
                      output_width, output_height);
   block2d_average_c(tmp, 64, dst_ptr, dst_stride,
                     output_width, output_height);
 }
