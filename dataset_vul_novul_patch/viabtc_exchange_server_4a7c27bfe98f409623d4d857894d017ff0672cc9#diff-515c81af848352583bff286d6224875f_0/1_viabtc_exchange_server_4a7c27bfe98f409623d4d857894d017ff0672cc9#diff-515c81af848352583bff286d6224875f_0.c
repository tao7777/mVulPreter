void nw_cache_free(nw_cache *cache, void *obj)
 {
     if (cache->free < cache->free_total) {
         cache->free_arr[cache->free++] = obj;
    } else {
         uint32_t new_free_total = cache->free_total * 2;
         void *new_arr = realloc(cache->free_arr, new_free_total * sizeof(void *));
         if (new_arr) {
            cache->free_total = new_free_total;
            cache->free_arr = new_arr;
            cache->free_arr[cache->free++] = obj;
         } else {
             free(obj);
         }
     }
 }
