VOID ixheaacd_shiftrountine_with_rnd_hq(WORD32 *qmf_real, WORD32 *qmf_imag,
                                        WORD32 *filter_states, WORD32 len,
                                        WORD32 shift) {
  WORD32 *filter_states_rev = filter_states + len;
  WORD32 treal, timag;
  WORD32 j;

 for (j = (len - 1); j >= 0; j -= 2) {
    WORD32 r1, r2, i1, i2;
    i2 = qmf_imag[j];
    r2 = qmf_real[j];

     r1 = *qmf_real++;
     i1 = *qmf_imag++;
 
    timag = ixheaacd_add32(i1, r1);
     timag = (ixheaacd_shl32_sat(timag, shift));
     filter_states_rev[j] = timag;
 
    treal = ixheaacd_sub32(i2, r2);
     treal = (ixheaacd_shl32_sat(treal, shift));
     filter_states[j] = treal;
 
    treal = ixheaacd_sub32(i1, r1);
     treal = (ixheaacd_shl32_sat(treal, shift));
     *filter_states++ = treal;
 
    timag = ixheaacd_add32(i2, r2);
     timag = (ixheaacd_shl32_sat(timag, shift));
     *filter_states_rev++ = timag;
   }
}
