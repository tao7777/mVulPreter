void fht16x16_ref(const int16_t *in, int16_t *out, int stride, int tx_type) {
   vp9_fht16x16_c(in, out, stride, tx_type);
 }
