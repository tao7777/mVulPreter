 do_prefetch_tables (const void *gcmM, size_t gcmM_size)
 {
  /* Modify counters to trigger copy-on-write and unsharing if physical pages
   * of look-up table are shared between processes.  Modifying counters also
   * causes checksums for pages to change and hint same-page merging algorithm
   * that these pages are frequently changing.  */
  gcm_table.counter_head++;
  gcm_table.counter_tail++;

  /* Prefetch look-up tables to cache.  */
   prefetch_table(gcmM, gcmM_size);
  prefetch_table(&gcm_table, sizeof(gcm_table));
 }
