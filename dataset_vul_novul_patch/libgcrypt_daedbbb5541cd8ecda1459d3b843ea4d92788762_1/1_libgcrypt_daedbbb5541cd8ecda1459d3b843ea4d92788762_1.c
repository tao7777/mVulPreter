 static void prefetch_dec(void)
 {
   prefetch_table((const void *)&dec_tables, sizeof(dec_tables));
 }
