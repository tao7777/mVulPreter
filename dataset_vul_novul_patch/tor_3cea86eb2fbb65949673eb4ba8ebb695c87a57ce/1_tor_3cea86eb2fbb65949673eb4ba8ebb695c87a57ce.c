 chunk_grow(chunk_t *chunk, size_t sz)
 {
   off_t offset;
  size_t memlen_orig = chunk->memlen;
   tor_assert(sz > chunk->memlen);
   offset = chunk->data - chunk->mem;
  chunk = tor_realloc(chunk, CHUNK_ALLOC_SIZE(sz));
   chunk->memlen = sz;
   chunk->data = chunk->mem + offset;
 #ifdef DEBUG_CHUNK_ALLOC
  tor_assert(chunk->DBG_alloc == CHUNK_ALLOC_SIZE(memlen_orig));
  chunk->DBG_alloc = CHUNK_ALLOC_SIZE(sz);
 #endif
  total_bytes_allocated_in_chunks +=
    CHUNK_ALLOC_SIZE(sz) - CHUNK_ALLOC_SIZE(memlen_orig);
   return chunk;
 }
