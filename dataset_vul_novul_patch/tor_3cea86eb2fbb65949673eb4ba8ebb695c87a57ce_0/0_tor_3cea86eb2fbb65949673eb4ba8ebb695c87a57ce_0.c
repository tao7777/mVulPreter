chunk_new_with_alloc_size(size_t alloc)
{
  chunk_t *ch;
  ch = tor_malloc(alloc);
  ch->next = NULL;
  ch->datalen = 0;
#ifdef DEBUG_CHUNK_ALLOC
  ch->DBG_alloc = alloc;
#endif
   ch->memlen = CHUNK_SIZE_WITH_ALLOC(alloc);
   total_bytes_allocated_in_chunks += alloc;
   ch->data = &ch->mem[0];
  CHUNK_SET_SENTINEL(ch, alloc);
   return ch;
 }
