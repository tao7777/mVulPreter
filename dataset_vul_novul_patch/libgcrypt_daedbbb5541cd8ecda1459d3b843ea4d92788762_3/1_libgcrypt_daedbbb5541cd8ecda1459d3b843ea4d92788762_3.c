static void prefetch_table(const volatile byte *tab, size_t len)
 {
   size_t i;
 
  for (i = 0; i < len; i += 8 * 32)
     {
       (void)tab[i + 0 * 32];
       (void)tab[i + 1 * 32];
      (void)tab[i + 2 * 32];
      (void)tab[i + 3 * 32];
      (void)tab[i + 4 * 32];
      (void)tab[i + 5 * 32];
       (void)tab[i + 6 * 32];
       (void)tab[i + 7 * 32];
     }
 
   (void)tab[len - 1];
 }
