void fdct4x4_ref(const int16_t *in, int16_t *out, int stride, int tx_type) {
void fdct4x4_ref(const int16_t *in, tran_low_t *out, int stride,
                 int tx_type) {
  vpx_fdct4x4_c(in, out, stride);
 }
