static unsigned int variance_ref(const uint8_t *ref, const uint8_t *src,
                                 int l2w, int l2h, unsigned int *sse_ptr) {
  int se = 0;
  unsigned int sse = 0;
  const int w = 1 << l2w, h = 1 << l2h;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int diff = ref[w * y + x] - src[w * y + x];
      se += diff;
      sse += diff * diff;
    }
//// Truncate high bit depth results by downshifting (with rounding) by:
//// 2 * (bit_depth - 8) for sse
//// (bit_depth - 8) for se
   }
  *sse_ptr = sse;
  return sse - (((int64_t) se * se) >> (l2w + l2h));
 }
