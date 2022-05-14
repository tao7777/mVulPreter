static __forceinline void draw_line(float *output, int x0, int y0, int x1, int y1, int n)
{
   int dy = y1 - y0;
   int adx = x1 - x0;
   int ady = abs(dy);
   int base;
   int x=x0,y=y0;
   int err = 0;
   int sy;

#ifdef STB_VORBIS_DIVIDE_TABLE
   if (adx < DIVTAB_DENOM && ady < DIVTAB_NUMER) {
      if (dy < 0) {
         base = -integer_divide_table[ady][adx];
         sy = base-1;
      } else {
         base =  integer_divide_table[ady][adx];
         sy = base+1;
      }
   } else {
      base = dy / adx;
      if (dy < 0)
         sy = base - 1;
      else
         sy = base+1;
   }
#else
   base = dy / adx;
   if (dy < 0)
      sy = base - 1;
   else
      sy = base+1;
#endif
    ady -= abs(base) * adx;
    if (x1 > n) x1 = n;
    if (x < x1) {
      LINE_OP(output[x], inverse_db_table[y&255]);
       for (++x; x < x1; ++x) {
          err += ady;
          if (err >= adx) {
             err -= adx;
             y += sy;
          } else
             y += base;
         LINE_OP(output[x], inverse_db_table[y&255]);
       }
    }
 }
