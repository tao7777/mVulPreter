VOID ixheaacd_esbr_radix4bfly(const WORD32 *w, WORD32 *x, WORD32 index1,

                               WORD32 index) {
   int i;
   WORD32 l1, l2, h2, fft_jmp;
  WORD64 xt0_0, yt0_0, xt1_0, yt1_0, xt2_0, yt2_0;
  WORD64 xh0_0, xh1_0, xh20_0, xh21_0, xl0_0, xl1_0, xl20_0, xl21_0;
   WORD32 x_0, x_1, x_l1_0, x_l1_1, x_l2_0, x_l2_1;
   WORD32 x_h2_0, x_h2_1;
   WORD32 si10, si20, si30, co10, co20, co30;

  WORD64 mul_1, mul_2, mul_3, mul_4, mul_5, mul_6;
  WORD64 mul_7, mul_8, mul_9, mul_10, mul_11, mul_12;
  WORD32 *x_l1;
  WORD32 *x_l2;
  WORD32 *x_h2;
 const WORD32 *w_ptr = w;
  WORD32 i1;

  h2 = index << 1;
  l1 = index << 2;
  l2 = (index << 2) + (index << 1);

  x_l1 = &(x[l1]);
  x_l2 = &(x[l2]);
  x_h2 = &(x[h2]);

  fft_jmp = 6 * (index);

 for (i1 = 0; i1 < index1; i1++) {
 for (i = 0; i < index; i++) {
      si10 = (*w_ptr++);
      co10 = (*w_ptr++);
      si20 = (*w_ptr++);
      co20 = (*w_ptr++);
      si30 = (*w_ptr++);
      co30 = (*w_ptr++);

      x_0 = x[0];
      x_h2_0 = x[h2];

       x_l1_0 = x[l1];
       x_l2_0 = x[l2];
 
      xh0_0 = (WORD64)x_0 + (WORD64)x_l1_0;
      xl0_0 = (WORD64)x_0 - (WORD64)x_l1_0;
 
      xh20_0 = (WORD64)x_h2_0 + (WORD64)x_l2_0;
      xl20_0 = (WORD64)x_h2_0 - (WORD64)x_l2_0;
 
      x[0] = (WORD32)ixheaacd_add64_sat(xh0_0, xh20_0);
      xt0_0 = (WORD64)xh0_0 - (WORD64)xh20_0;
 
       x_1 = x[1];
       x_h2_1 = x[h2 + 1];
       x_l1_1 = x[l1 + 1];
       x_l2_1 = x[l2 + 1];
 
      xh1_0 = (WORD64)x_1 + (WORD64)x_l1_1;
      xl1_0 = (WORD64)x_1 - (WORD64)x_l1_1;
 
      xh21_0 = (WORD64)x_h2_1 + (WORD64)x_l2_1;
      xl21_0 = (WORD64)x_h2_1 - (WORD64)x_l2_1;
 
      x[1] = (WORD32)ixheaacd_add64_sat(xh1_0, xh21_0);
      yt0_0 = (WORD64)xh1_0 - (WORD64)xh21_0;
 
      xt1_0 = (WORD64)xl0_0 + (WORD64)xl21_0;
      xt2_0 = (WORD64)xl0_0 - (WORD64)xl21_0;
 
      yt2_0 = (WORD64)xl1_0 + (WORD64)xl20_0;
      yt1_0 = (WORD64)xl1_0 - (WORD64)xl20_0;
 
       mul_11 = ixheaacd_mult64(xt2_0, co30);
       mul_3 = ixheaacd_mult64(yt2_0, si30);
      x[l2] = (WORD32)((mul_3 + mul_11) >> 32) << RADIXSHIFT;

      mul_5 = ixheaacd_mult64(xt2_0, si30);
      mul_9 = ixheaacd_mult64(yt2_0, co30);
      x[l2 + 1] = (WORD32)((mul_9 - mul_5) >> 32) << RADIXSHIFT;

      mul_12 = ixheaacd_mult64(xt0_0, co20);
      mul_2 = ixheaacd_mult64(yt0_0, si20);
      x[l1] = (WORD32)((mul_2 + mul_12) >> 32) << RADIXSHIFT;

      mul_6 = ixheaacd_mult64(xt0_0, si20);
      mul_8 = ixheaacd_mult64(yt0_0, co20);
      x[l1 + 1] = (WORD32)((mul_8 - mul_6) >> 32) << RADIXSHIFT;

      mul_4 = ixheaacd_mult64(xt1_0, co10);
      mul_1 = ixheaacd_mult64(yt1_0, si10);
      x[h2] = (WORD32)((mul_1 + mul_4) >> 32) << RADIXSHIFT;

      mul_10 = ixheaacd_mult64(xt1_0, si10);
      mul_7 = ixheaacd_mult64(yt1_0, co10);
      x[h2 + 1] = (WORD32)((mul_7 - mul_10) >> 32) << RADIXSHIFT;

      x += 2;
 }
    x += fft_jmp;
    w_ptr = w_ptr - fft_jmp;
 }
}
