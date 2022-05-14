void reference_32x32_dct_1d(const double in[32], double out[32], int stride) {
   const double kInvSqrt2 = 0.707106781186547524400844362104;
   for (int k = 0; k < 32; k++) {
     out[k] = 0.0;
 for (int n = 0; n < 32; n++)
      out[k] += in[n] * cos(kPi * (2 * n + 1) * k / 64.0);
 if (k == 0)
      out[k] = out[k] * kInvSqrt2;
 }
}
