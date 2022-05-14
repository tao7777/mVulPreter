VOID ixheaacd_esbr_postradixcompute2(WORD32 *ptr_y, WORD32 *ptr_x,
 const WORD32 *pdig_rev_tbl,
                                     WORD32 npoints) {
  WORD32 i, k;
  WORD32 h2;

   WORD32 x_0, x_1, x_2, x_3;
   WORD32 x_4, x_5, x_6, x_7;
   WORD32 x_8, x_9, x_a, x_b, x_c, x_d, x_e, x_f;
   WORD32 n0, j0;
   WORD32 *x2, *x0;
   WORD32 *y0, *y1, *y2, *y3;

  y0 = ptr_y;
  y2 = ptr_y + (WORD32)npoints;
  x0 = ptr_x;
  x2 = ptr_x + (WORD32)(npoints >> 1);

  y1 = y0 + (WORD32)(npoints >> 2);
  y3 = y2 + (WORD32)(npoints >> 2);
  j0 = 8;
  n0 = npoints >> 1;

 for (k = 0; k < 2; k++) {
 for (i = 0; i<npoints>> 1; i += 8) {
      h2 = *pdig_rev_tbl++ >> 2;

      x_0 = *x0++;
      x_1 = *x0++;
      x_2 = *x0++;
      x_3 = *x0++;
      x_4 = *x0++;
      x_5 = *x0++;

       x_6 = *x0++;
       x_7 = *x0++;
 
      y0[h2] = ixheaacd_add32_sat(x_0, x_2);
      y0[h2 + 1] = ixheaacd_add32_sat(x_1, x_3);
      y1[h2] = ixheaacd_add32_sat(x_4, x_6);
      y1[h2 + 1] = ixheaacd_add32_sat(x_5, x_7);
      y2[h2] = ixheaacd_sub32_sat(x_0, x_2);
      y2[h2 + 1] = ixheaacd_sub32_sat(x_1, x_3);
      y3[h2] = ixheaacd_sub32_sat(x_4, x_6);
      y3[h2 + 1] = ixheaacd_sub32_sat(x_5, x_7);
 
       x_8 = *x2++;
       x_9 = *x2++;
      x_a = *x2++;
      x_b = *x2++;
      x_c = *x2++;
      x_d = *x2++;

       x_e = *x2++;
       x_f = *x2++;
 
      y0[h2 + 2] = ixheaacd_add32_sat(x_8, x_a);
      y0[h2 + 3] = ixheaacd_add32_sat(x_9, x_b);
      y1[h2 + 2] = ixheaacd_add32_sat(x_c, x_e);
      y1[h2 + 3] = ixheaacd_add32_sat(x_d, x_f);
      y2[h2 + 2] = ixheaacd_sub32_sat(x_8, x_a);
      y2[h2 + 3] = ixheaacd_sub32_sat(x_9, x_b);
      y3[h2 + 2] = ixheaacd_sub32_sat(x_c, x_e);
      y3[h2 + 3] = ixheaacd_sub32_sat(x_d, x_f);
     }
     x0 += (WORD32)npoints >> 1;
     x2 += (WORD32)npoints >> 1;
 }
}
