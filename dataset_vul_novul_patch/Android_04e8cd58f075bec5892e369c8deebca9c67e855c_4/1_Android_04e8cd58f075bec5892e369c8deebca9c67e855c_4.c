WORD32 ixheaacd_qmf_hbe_data_reinit(ia_esbr_hbe_txposer_struct *ptr_hbe_txposer,
                                    WORD16 *p_freq_band_tab[2],
                                    WORD16 *p_num_sfb, WORD32 upsamp_4_flag) {
  WORD32 synth_size, sfb, patch, stop_patch;

 if (ptr_hbe_txposer != NULL) {
    ptr_hbe_txposer->start_band = p_freq_band_tab[LOW][0];
    ptr_hbe_txposer->end_band = p_freq_band_tab[LOW][p_num_sfb[LOW]];

    ptr_hbe_txposer->synth_size =
 4 * ((ptr_hbe_txposer->start_band + 4) / 8 + 1);
    ptr_hbe_txposer->k_start =
        ixheaacd_start_subband2kL_tbl[ptr_hbe_txposer->start_band];

    ptr_hbe_txposer->upsamp_4_flag = upsamp_4_flag;

 if (upsamp_4_flag) {
 if (ptr_hbe_txposer->k_start + ptr_hbe_txposer->synth_size > 16)
        ptr_hbe_txposer->k_start = 16 - ptr_hbe_txposer->synth_size;
 } else if (ptr_hbe_txposer->core_frame_length == 768) {
 if (ptr_hbe_txposer->k_start + ptr_hbe_txposer->synth_size > 24)
        ptr_hbe_txposer->k_start = 24 - ptr_hbe_txposer->synth_size;
 }

    memset(ptr_hbe_txposer->synth_buf, 0, 1280 * sizeof(FLOAT32));
    synth_size = ptr_hbe_txposer->synth_size;
    ptr_hbe_txposer->synth_buf_offset = 18 * synth_size;
 switch (synth_size) {
 case 4:
        ptr_hbe_txposer->synth_cos_tab =

             (FLOAT32 *)ixheaacd_synth_cos_table_kl_4;
         ptr_hbe_txposer->analy_cos_sin_tab =
             (FLOAT32 *)ixheaacd_analy_cos_sin_table_kl_8;
        ixheaacd_real_synth_fft = &ixheaacd_real_synth_fft_p2;
        ixheaacd_cmplx_anal_fft = &ixheaacd_cmplx_anal_fft_p2;
         break;
       case 8:
         ptr_hbe_txposer->synth_cos_tab =
             (FLOAT32 *)ixheaacd_synth_cos_table_kl_8;
         ptr_hbe_txposer->analy_cos_sin_tab =
             (FLOAT32 *)ixheaacd_analy_cos_sin_table_kl_16;
        ixheaacd_real_synth_fft = &ixheaacd_real_synth_fft_p2;
        ixheaacd_cmplx_anal_fft = &ixheaacd_cmplx_anal_fft_p2;
         break;
       case 12:
         ptr_hbe_txposer->synth_cos_tab =
             (FLOAT32 *)ixheaacd_synth_cos_table_kl_12;
         ptr_hbe_txposer->analy_cos_sin_tab =
             (FLOAT32 *)ixheaacd_analy_cos_sin_table_kl_24;
        ixheaacd_real_synth_fft = &ixheaacd_real_synth_fft_p3;
        ixheaacd_cmplx_anal_fft = &ixheaacd_cmplx_anal_fft_p3;
         break;
       case 16:
         ptr_hbe_txposer->synth_cos_tab =
             (FLOAT32 *)ixheaacd_synth_cos_table_kl_16;
         ptr_hbe_txposer->analy_cos_sin_tab =
             (FLOAT32 *)ixheaacd_analy_cos_sin_table_kl_32;
        ixheaacd_real_synth_fft = &ixheaacd_real_synth_fft_p2;
        ixheaacd_cmplx_anal_fft = &ixheaacd_cmplx_anal_fft_p2;
         break;
       case 20:
         ptr_hbe_txposer->synth_cos_tab =
 (FLOAT32 *)ixheaacd_synth_cos_table_kl_20;
        ptr_hbe_txposer->analy_cos_sin_tab =
 (FLOAT32 *)ixheaacd_analy_cos_sin_table_kl_40;
 break;
 default:
        ptr_hbe_txposer->synth_cos_tab =

             (FLOAT32 *)ixheaacd_synth_cos_table_kl_4;
         ptr_hbe_txposer->analy_cos_sin_tab =
             (FLOAT32 *)ixheaacd_analy_cos_sin_table_kl_8;
        ixheaacd_real_synth_fft = &ixheaacd_real_synth_fft_p2;
        ixheaacd_cmplx_anal_fft = &ixheaacd_cmplx_anal_fft_p2;
     }
 
     ptr_hbe_txposer->synth_wind_coeff = ixheaacd_map_prot_filter(synth_size);

    memset(ptr_hbe_txposer->analy_buf, 0, 640 * sizeof(FLOAT32));
    synth_size = 2 * ptr_hbe_txposer->synth_size;
    ptr_hbe_txposer->analy_wind_coeff = ixheaacd_map_prot_filter(synth_size);

    memset(ptr_hbe_txposer->x_over_qmf, 0, MAX_NUM_PATCHES * sizeof(WORD32));
    sfb = 0;
 if (upsamp_4_flag) {
      stop_patch = MAX_NUM_PATCHES;
      ptr_hbe_txposer->max_stretch = MAX_STRETCH;
 } else {
      stop_patch = MAX_STRETCH;
 }

 for (patch = 1; patch <= stop_patch; patch++) {
 while (sfb <= p_num_sfb[LOW] &&
             p_freq_band_tab[LOW][sfb] <= patch * ptr_hbe_txposer->start_band)
        sfb++;
 if (sfb <= p_num_sfb[LOW]) {
 if ((patch * ptr_hbe_txposer->start_band -
             p_freq_band_tab[LOW][sfb - 1]) <= 3) {
          ptr_hbe_txposer->x_over_qmf[patch - 1] =
              p_freq_band_tab[LOW][sfb - 1];
 } else {
          WORD32 sfb = 0;
 while (sfb <= p_num_sfb[HIGH] &&
                 p_freq_band_tab[HIGH][sfb] <=
                     patch * ptr_hbe_txposer->start_band)
            sfb++;
          ptr_hbe_txposer->x_over_qmf[patch - 1] =
              p_freq_band_tab[HIGH][sfb - 1];
 }
 } else {
        ptr_hbe_txposer->x_over_qmf[patch - 1] = ptr_hbe_txposer->end_band;
        ptr_hbe_txposer->max_stretch = min(patch, MAX_STRETCH);

         break;
       }
     }
  }
  if (ptr_hbe_txposer->k_start < 0) {
    return -1;
   }
   return 0;
 }
