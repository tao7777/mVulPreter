WORD32 ixheaacd_complex_anal_filt(ia_esbr_hbe_txposer_struct *ptr_hbe_txposer) {
  WORD32 idx;
  WORD32 anal_size = 2 * ptr_hbe_txposer->synth_size;
  WORD32 N = (10 * anal_size);

 for (idx = 0; idx < (ptr_hbe_txposer->no_bins >> 1); idx++) {
    WORD32 i, j, k, l;
    FLOAT32 window_output[640];
    FLOAT32 u[128], u_in[256], u_out[256];
    FLOAT32 accu_r, accu_i;
 const FLOAT32 *inp_signal;
    FLOAT32 *anal_buf;

    FLOAT32 *analy_cos_sin_tab = ptr_hbe_txposer->analy_cos_sin_tab;
 const FLOAT32 *interp_window_coeff = ptr_hbe_txposer->analy_wind_coeff;
    FLOAT32 *x = ptr_hbe_txposer->analy_buf;

    memset(ptr_hbe_txposer->qmf_in_buf[idx + HBE_OPER_WIN_LEN - 1], 0,
           TWICE_QMF_SYNTH_CHANNELS_NUM * sizeof(FLOAT32));

    inp_signal = ptr_hbe_txposer->ptr_input_buf +
                 idx * 2 * ptr_hbe_txposer->synth_size + 1;
    anal_buf = &ptr_hbe_txposer->qmf_in_buf[idx + HBE_OPER_WIN_LEN - 1]
 [4 * ptr_hbe_txposer->k_start];

 for (i = N - 1; i >= anal_size; i--) {
      x[i] = x[i - anal_size];
 }

 for (i = anal_size - 1; i >= 0; i--) {
      x[i] = inp_signal[anal_size - 1 - i];
 }

 for (i = 0; i < N; i++) {
      window_output[i] = x[i] * interp_window_coeff[i];
 }

 for (i = 0; i < 2 * anal_size; i++) {
      accu_r = 0.0;
 for (j = 0; j < 5; j++) {
        accu_r = accu_r + window_output[i + j * 2 * anal_size];
 }
      u[i] = accu_r;
 }

 if (anal_size == 40) {
 for (i = 1; i < anal_size; i++) {
        FLOAT32 temp1 = u[i] + u[2 * anal_size - i];
        FLOAT32 temp2 = u[i] - u[2 * anal_size - i];
        u[i] = temp1;
        u[2 * anal_size - i] = temp2;
 }

 for (k = 0; k < anal_size; k++) {
        accu_r = u[anal_size];
 if (k & 1)
          accu_i = u[0];
 else
          accu_i = -u[0];
 for (l = 1; l < anal_size; l++) {
          accu_r = accu_r + u[0 + l] * analy_cos_sin_tab[2 * l + 0];
          accu_i = accu_i + u[2 * anal_size - l] * analy_cos_sin_tab[2 * l + 1];
 }
        analy_cos_sin_tab += (2 * anal_size);
 *anal_buf++ = (FLOAT32)accu_r;
 *anal_buf++ = (FLOAT32)accu_i;
 }
 } else {
      FLOAT32 *ptr_u = u_in;
      FLOAT32 *ptr_v = u_out;
 for (k = 0; k < anal_size * 2; k++) {

         *ptr_u++ = ((*analy_cos_sin_tab++) * u[k]);
         *ptr_u++ = ((*analy_cos_sin_tab++) * u[k]);
       }
      if (ptr_hbe_txposer->ixheaacd_cmplx_anal_fft != NULL)
        (*(ptr_hbe_txposer->ixheaacd_cmplx_anal_fft))(u_in, u_out,
                                                      anal_size * 2);
       else
         return -1;
 
 for (k = 0; k < anal_size / 2; k++) {
 *(anal_buf + 1) = -*ptr_v++;
 *anal_buf = *ptr_v++;

        anal_buf += 2;

 *(anal_buf + 1) = *ptr_v++;
 *anal_buf = -*ptr_v++;

        anal_buf += 2;
 }
 }
 }
 return 0;
}
