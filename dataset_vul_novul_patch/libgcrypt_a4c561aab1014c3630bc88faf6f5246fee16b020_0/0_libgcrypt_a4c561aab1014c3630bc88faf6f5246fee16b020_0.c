 void prefetch_table(const void *tab, size_t len)
 {
   const volatile byte *vtab = tab;
   size_t i;
 
  for (i = 0; len - i >= 8 * 32; i += 8 * 32)
     {
       (void)vtab[i + 0 * 32];
       (void)vtab[i + 1 * 32];
      (void)vtab[i + 2 * 32];
      (void)vtab[i + 3 * 32];
      (void)vtab[i + 4 * 32];
      (void)vtab[i + 5 * 32];
       (void)vtab[i + 6 * 32];
       (void)vtab[i + 7 * 32];
     }
  for (; i < len; i += 32)
    {
      (void)vtab[i];
    }
 
   (void)vtab[len - 1];
 }
