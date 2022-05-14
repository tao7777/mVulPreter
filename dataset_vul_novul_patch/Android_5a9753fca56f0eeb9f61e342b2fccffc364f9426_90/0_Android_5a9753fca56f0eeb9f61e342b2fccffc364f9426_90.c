void fht8x8_ref(const int16_t *in, int16_t *out, int stride, int tx_type) {
void reference_8x8_dct_2d(const int16_t input[kNumCoeffs],
                          double output[kNumCoeffs]) {
  // First transform columns
  for (int i = 0; i < 8; ++i) {
    double temp_in[8], temp_out[8];
    for (int j = 0; j < 8; ++j)
      temp_in[j] = input[j*8 + i];
    reference_8x8_dct_1d(temp_in, temp_out, 1);
    for (int j = 0; j < 8; ++j)
      output[j * 8 + i] = temp_out[j];
  }
  // Then transform rows
  for (int i = 0; i < 8; ++i) {
    double temp_in[8], temp_out[8];
    for (int j = 0; j < 8; ++j)
      temp_in[j] = output[j + i*8];
    reference_8x8_dct_1d(temp_in, temp_out, 1);
    // Scale by some magic number
    for (int j = 0; j < 8; ++j)
      output[j + i * 8] = temp_out[j] * 2;
  }
}


void fdct8x8_ref(const int16_t *in, tran_low_t *out, int stride, int tx_type) {
  vpx_fdct8x8_c(in, out, stride);
}

void fht8x8_ref(const int16_t *in, tran_low_t *out, int stride, int tx_type) {
   vp9_fht8x8_c(in, out, stride, tx_type);
 }
