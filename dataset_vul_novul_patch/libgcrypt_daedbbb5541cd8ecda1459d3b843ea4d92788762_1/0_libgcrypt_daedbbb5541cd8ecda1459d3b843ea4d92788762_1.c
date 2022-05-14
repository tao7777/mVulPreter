 static void prefetch_dec(void)
 {
  /* Modify counters to trigger copy-on-write and unsharing if physical pages
   * of look-up table are shared between processes.  Modifying counters also
   * causes checksums for pages to change and hint same-page merging algorithm
   * that these pages are frequently changing.  */
  dec_tables.counter_head++;
  dec_tables.counter_tail++;

  /* Prefetch look-up tables to cache.  */
   prefetch_table((const void *)&dec_tables, sizeof(dec_tables));
 }
