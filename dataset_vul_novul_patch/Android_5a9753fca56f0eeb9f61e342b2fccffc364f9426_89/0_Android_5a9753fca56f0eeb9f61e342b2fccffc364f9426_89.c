void fdct8x8_ref(const int16_t *in, int16_t *out, int stride, int tx_type) {
const int kSignBiasMaxDiff255 = 1500;
const int kSignBiasMaxDiff15 = 10000;

typedef void (*FdctFunc)(const int16_t *in, tran_low_t *out, int stride);
typedef void (*IdctFunc)(const tran_low_t *in, uint8_t *out, int stride);
typedef void (*FhtFunc)(const int16_t *in, tran_low_t *out, int stride,
                        int tx_type);
typedef void (*IhtFunc)(const tran_low_t *in, uint8_t *out, int stride,
                        int tx_type);

typedef std::tr1::tuple<FdctFunc, IdctFunc, int, vpx_bit_depth_t> Dct8x8Param;
typedef std::tr1::tuple<FhtFunc, IhtFunc, int, vpx_bit_depth_t> Ht8x8Param;
typedef std::tr1::tuple<IdctFunc, IdctFunc, int, vpx_bit_depth_t> Idct8x8Param;

void reference_8x8_dct_1d(const double in[8], double out[8], int stride) {
  const double kInvSqrt2 = 0.707106781186547524400844362104;
  for (int k = 0; k < 8; k++) {
    out[k] = 0.0;
    for (int n = 0; n < 8; n++)
      out[k] += in[n] * cos(kPi * (2 * n + 1) * k / 16.0);
    if (k == 0)
      out[k] = out[k] * kInvSqrt2;
  }
 }
