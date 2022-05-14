store_pool_delete(png_store *ps, store_pool *pool)
{
 if (pool->list != NULL)
 {
      fprintf(stderr, "%s: %s %s: memory lost (list follows):\n", ps->test,
         pool == &ps->read_memory_pool ? "read" : "write",
         pool == &ps->read_memory_pool ? (ps->current != NULL ?
            ps->current->name : "unknown file") : ps->wname);
 ++ps->nerrors;

 do
 {
         store_memory *next = pool->list;
         pool->list = next->next;

          next->next = NULL;
 
          fprintf(stderr, "\t%lu bytes @ %p\n",
             (unsigned long)next->size, (const void*)(next+1));
          /* The NULL means this will always return, even if the memory is
           * corrupted.
           */
         store_memory_free(NULL, pool, next);
 }
 while (pool->list != NULL);
 }

 /* And reset the other fields too for the next time. */
 if (pool->max > pool->max_max) pool->max_max = pool->max;
   pool->max = 0;
 if (pool->current != 0) /* unexpected internal error */
      fprintf(stderr, "%s: %s %s: memory counter mismatch (internal error)\n",
         ps->test, pool == &ps->read_memory_pool ? "read" : "write",
         pool == &ps->read_memory_pool ? (ps->current != NULL ?
            ps->current->name : "unknown file") : ps->wname);
   pool->current = 0;

 if (pool->limit > pool->max_limit)
      pool->max_limit = pool->limit;

   pool->limit = 0;

 if (pool->total > pool->max_total)
      pool->max_total = pool->total;

   pool->total = 0;

 /* Get a new mark too. */
   store_pool_mark(pool->mark);
}
