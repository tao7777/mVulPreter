void nw_buf_free(nw_buf_pool *pool, nw_buf *buf)
 {
     if (pool->free < pool->free_total) {
         pool->free_arr[pool->free++] = buf;
    } else if (pool->free_total < NW_BUF_POOL_MAX_SIZE) {
         uint32_t new_free_total = pool->free_total * 2;
         void *new_arr = realloc(pool->free_arr, new_free_total * sizeof(nw_buf *));
         if (new_arr) {
            pool->free_total = new_free_total;
            pool->free_arr = new_arr;
            pool->free_arr[pool->free++] = buf;
         } else {
             free(buf);
         }
    } else {
        free(buf);
     }
 }
