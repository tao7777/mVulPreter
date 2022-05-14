static unsigned int variance_ref(const uint8_t *ref, const uint8_t *src,
//// Truncate high bit depth results by downshifting (with rounding) by:
//// 2 * (bit_depth - 8) for sse
//// (bit_depth - 8) for se
static void RoundHighBitDepth(int bit_depth, int64_t *se, uint64_t *sse) {
  switch (bit_depth) {
    case VPX_BITS_12:
      *sse = (*sse + 128) >> 8;
      *se = (*se + 8) >> 4;
      break;
    case VPX_BITS_10:
      *sse = (*sse + 8) >> 4;
      *se = (*se + 2) >> 2;
      break;
    case VPX_BITS_8:
    default:
      break;
   }
 }
