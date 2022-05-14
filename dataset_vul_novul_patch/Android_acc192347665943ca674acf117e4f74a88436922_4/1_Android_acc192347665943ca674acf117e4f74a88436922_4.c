static void copyMultiCh24(short *dst, const int *const *src, unsigned nSamples, unsigned nChannels)
 {
     for (unsigned i = 0; i < nSamples; ++i) {
         for (unsigned c = 0; c < nChannels; ++c) {
 *dst++ = src[c][i] >> 8;
 }
 }
}
