void fht4x4_ref(const int16_t *in, int16_t *out, int stride, int tx_type) {
   vp9_fht4x4_c(in, out, stride, tx_type);
 }
