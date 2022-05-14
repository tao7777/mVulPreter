void reference_32x32_dct_2d(const int16_t input[kNumCoeffs],
 double output[kNumCoeffs]) {
 for (int i = 0; i < 32; ++i) {

     double temp_in[32], temp_out[32];
     for (int j = 0; j < 32; ++j)
       temp_in[j] = input[j*32 + i];
    reference_32x32_dct_1d(temp_in, temp_out);
     for (int j = 0; j < 32; ++j)
       output[j * 32 + i] = temp_out[j];
   }
 for (int i = 0; i < 32; ++i) {

     double temp_in[32], temp_out[32];
     for (int j = 0; j < 32; ++j)
       temp_in[j] = output[j + i*32];
    reference_32x32_dct_1d(temp_in, temp_out);
     for (int j = 0; j < 32; ++j)
       output[j + i * 32] = temp_out[j] / 4;
   }
 }
