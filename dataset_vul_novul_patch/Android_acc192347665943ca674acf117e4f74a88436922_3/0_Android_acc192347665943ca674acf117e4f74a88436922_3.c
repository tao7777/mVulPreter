static void copyMultiCh16(short *dst, const int *const *src, unsigned nSamples, unsigned nChannels)
static void copyMultiCh16(short *dst, const int * src[FLACParser::kMaxChannels], unsigned nSamples, unsigned nChannels)
 {
     for (unsigned i = 0; i < nSamples; ++i) {
         for (unsigned c = 0; c < nChannels; ++c) {
 *dst++ = src[c][i];
 }
 }
}
