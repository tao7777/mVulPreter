VOID ixheaacd_esbr_postradixcompute2(WORD32 *ptr_y, WORD32 *ptr_x,
 const WORD32 *pdig_rev_tbl,
                                     WORD32 npoints) {
  WORD32 i, k;
  WORD32 h2;

   WORD32 x_0, x_1, x_2, x_3;
   WORD32 x_4, x_5, x_6, x_7;
   WORD32 x_8, x_9, x_a, x_b, x_c, x_d, x_e, x_f;
  WORD32 n00, n10, n20, n30, n01, n11, n21, n31;
  WORD32 n02, n12, n22, n32, n03, n13, n23, n33;
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
 
      n00 = x_0 + x_2;
      n01 = x_1 + x_3;
      n20 = x_0 - x_2;
      n21 = x_1 - x_3;
      n10 = x_4 + x_6;
      n11 = x_5 + x_7;
      n30 = x_4 - x_6;
      n31 = x_5 - x_7;
      y0[h2] = n00;
      y0[h2 + 1] = n01;
      y1[h2] = n10;
      y1[h2 + 1] = n11;
      y2[h2] = n20;
      y2[h2 + 1] = n21;
      y3[h2] = n30;
      y3[h2 + 1] = n31;
 
       x_8 = *x2++;
       x_9 = *x2++;
      x_a = *x2++;
      x_b = *x2++;
      x_c = *x2++;
      x_d = *x2++;

       x_e = *x2++;
       x_f = *x2++;
 
      n02 = x_8 + x_a;
      n03 = x_9 + x_b;
      n22 = x_8 - x_a;
      n23 = x_9 - x_b;
      n12 = x_c + x_e;
      n13 = x_d + x_f;
      n32 = x_c - x_e;
      n33 = x_d - x_f;
      y0[h2 + 2] = n02;
      y0[h2 + 3] = n03;
      y1[h2 + 2] = n12;
      y1[h2 + 3] = n13;
      y2[h2 + 2] = n22;
      y2[h2 + 3] = n23;
      y3[h2 + 2] = n32;
      y3[h2 + 3] = n33;
     }
     x0 += (WORD32)npoints >> 1;
     x2 += (WORD32)npoints >> 1;
 }
}
