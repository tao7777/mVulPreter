static unsigned int subpel_variance_ref(const uint8_t *ref, const uint8_t *src,
                                        int l2w, int l2h, int xoff, int yoff,
                                        unsigned int *sse_ptr) {
  int se = 0;
  unsigned int sse = 0;
  const int w = 1 << l2w, h = 1 << l2h;
   for (int y = 0; y < h; y++) {
     for (int x = 0; x < w; x++) {
      const int a1 = ref[(w + 1) * (y + 0) + x + 0];
      const int a2 = ref[(w + 1) * (y + 0) + x + 1];
      const int b1 = ref[(w + 1) * (y + 1) + x + 0];
      const int b2 = ref[(w + 1) * (y + 1) + x + 1];
      const int a = a1 + (((a2 - a1) * xoff + 8) >> 4);
      const int b = b1 + (((b2 - b1) * xoff + 8) >> 4);
      const int r = a + (((b - a) * yoff + 8) >> 4);
      int diff = r - src[w * y + x];
      se += diff;
      sse += diff * diff;
     }
   }
  *sse_ptr = sse;
  return sse - (((int64_t) se * se) >> (l2w + l2h));
 }
