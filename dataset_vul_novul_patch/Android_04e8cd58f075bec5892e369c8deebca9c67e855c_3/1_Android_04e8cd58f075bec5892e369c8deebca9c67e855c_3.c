WORD32 ixheaacd_real_synth_filt(ia_esbr_hbe_txposer_struct *ptr_hbe_txposer,
                                WORD32 num_columns, FLOAT32 qmf_buf_real[][64],
                                FLOAT32 qmf_buf_imag[][64]) {
  WORD32 i, j, k, l, idx;
  FLOAT32 g[640];
  FLOAT32 w[640];
  FLOAT32 synth_out[128];
  FLOAT32 accu_r;
  WORD32 synth_size = ptr_hbe_txposer->synth_size;
  FLOAT32 *ptr_cos_tab_trans_qmf =
 (FLOAT32 *)&ixheaacd_cos_table_trans_qmf[0][0] +
      ptr_hbe_txposer->k_start * 32;
  FLOAT32 *buffer = ptr_hbe_txposer->synth_buf;

 for (idx = 0; idx < num_columns; idx++) {
    FLOAT32 loc_qmf_buf[64];
    FLOAT32 *synth_buf_r = loc_qmf_buf;
    FLOAT32 *out_buf = ptr_hbe_txposer->ptr_input_buf +
 (idx + 1) * ptr_hbe_txposer->synth_size;
    FLOAT32 *synth_cos_tab = ptr_hbe_txposer->synth_cos_tab;
 const FLOAT32 *interp_window_coeff = ptr_hbe_txposer->synth_wind_coeff;
 if (ptr_hbe_txposer->k_start < 0) return -1;
 for (k = 0; k < synth_size; k++) {
      WORD32 ki = ptr_hbe_txposer->k_start + k;
      synth_buf_r[k] = (FLOAT32)(
          ptr_cos_tab_trans_qmf[(k << 1) + 0] * qmf_buf_real[idx][ki] +
          ptr_cos_tab_trans_qmf[(k << 1) + 1] * qmf_buf_imag[idx][ki]);

      synth_buf_r[k + ptr_hbe_txposer->synth_size] = 0;
 }

 for (l = (20 * synth_size - 1); l >= 2 * synth_size; l--) {
      buffer[l] = buffer[l - 2 * synth_size];
 }

 if (synth_size == 20) {
      FLOAT32 *psynth_cos_tab = synth_cos_tab;

 for (l = 0; l < (synth_size + 1); l++) {
        accu_r = 0.0;
 for (k = 0; k < synth_size; k++) {
          accu_r += synth_buf_r[k] * psynth_cos_tab[k];
 }
        buffer[0 + l] = accu_r;
        buffer[synth_size - l] = accu_r;
        psynth_cos_tab = psynth_cos_tab + synth_size;
 }
 for (l = (synth_size + 1); l < (2 * synth_size - synth_size / 2); l++) {
        accu_r = 0.0;
 for (k = 0; k < synth_size; k++) {
          accu_r += synth_buf_r[k] * psynth_cos_tab[k];
 }
        buffer[0 + l] = accu_r;
        buffer[3 * synth_size - l] = -accu_r;
        psynth_cos_tab = psynth_cos_tab + synth_size;
 }
      accu_r = 0.0;
 for (k = 0; k < synth_size; k++) {
        accu_r += synth_buf_r[k] * psynth_cos_tab[k];
 }
      buffer[3 * synth_size >> 1] = accu_r;
 } else {
      FLOAT32 tmp;
      FLOAT32 *ptr_u = synth_out;
      WORD32 kmax = (synth_size >> 1);

       FLOAT32 *syn_buf = &buffer[kmax];
       kmax += synth_size;
 
      if (ixheaacd_real_synth_fft != NULL)
        (*ixheaacd_real_synth_fft)(synth_buf_r, synth_out, synth_size * 2);
       else
         return -1;
 
 for (k = 0; k < kmax; k++) {
        tmp = ((*ptr_u++) * (*synth_cos_tab++));
        tmp -= ((*ptr_u++) * (*synth_cos_tab++));
 *syn_buf++ = tmp;
 }

      syn_buf = &buffer[0];
      kmax -= synth_size;

 for (k = 0; k < kmax; k++) {
        tmp = ((*ptr_u++) * (*synth_cos_tab++));
        tmp -= ((*ptr_u++) * (*synth_cos_tab++));
 *syn_buf++ = tmp;
 }
 }

 for (i = 0; i < 5; i++) {
      memcpy(&g[(2 * i + 0) * synth_size], &buffer[(4 * i + 0) * synth_size],
 sizeof(FLOAT32) * synth_size);
      memcpy(&g[(2 * i + 1) * synth_size], &buffer[(4 * i + 3) * synth_size],
 sizeof(FLOAT32) * synth_size);
 }

 for (k = 0; k < 10 * synth_size; k++) {
      w[k] = g[k] * interp_window_coeff[k];
 }

 for (i = 0; i < synth_size; i++) {
      accu_r = 0.0;
 for (j = 0; j < 10; j++) {
        accu_r = accu_r + w[synth_size * j + i];
 }
      out_buf[i] = (FLOAT32)accu_r;
 }
 }
 return 0;
}
