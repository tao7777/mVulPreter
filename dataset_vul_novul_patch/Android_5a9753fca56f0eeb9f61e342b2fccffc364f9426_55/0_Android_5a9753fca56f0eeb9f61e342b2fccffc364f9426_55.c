void fht16x16_ref(const int16_t *in, int16_t *out, int stride, int tx_type) {
void idct16x16_ref(const tran_low_t *in, uint8_t *dest, int stride,
                   int /*tx_type*/) {
  vpx_idct16x16_256_add_c(in, dest, stride);
}

void fht16x16_ref(const int16_t *in, tran_low_t *out, int stride,
                  int tx_type) {
   vp9_fht16x16_c(in, out, stride, tx_type);
 }
