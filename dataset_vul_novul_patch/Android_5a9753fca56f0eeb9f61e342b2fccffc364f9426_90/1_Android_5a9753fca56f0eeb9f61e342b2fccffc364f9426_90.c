void fht8x8_ref(const int16_t *in, int16_t *out, int stride, int tx_type) {
   vp9_fht8x8_c(in, out, stride, tx_type);
 }
