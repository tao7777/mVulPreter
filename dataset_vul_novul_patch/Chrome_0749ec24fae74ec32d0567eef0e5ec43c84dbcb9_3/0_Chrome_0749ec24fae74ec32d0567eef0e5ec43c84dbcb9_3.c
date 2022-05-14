bool NormalPageArena::coalesce() {
  if (m_promptlyFreedSize < 1024 * 1024)
    return false;

  if (getThreadState()->sweepForbidden())
    return false;

  ASSERT(!hasCurrentAllocationArea());
  TRACE_EVENT0("blink_gc", "BaseArena::coalesce");

  m_freeList.clear();
  size_t freedSize = 0;
  for (NormalPage* page = static_cast<NormalPage*>(m_firstPage); page;
       page = static_cast<NormalPage*>(page->next())) {
    Address startOfGap = page->payload();
    for (Address headerAddress = startOfGap;
         headerAddress < page->payloadEnd();) {
      HeapObjectHeader* header =
          reinterpret_cast<HeapObjectHeader*>(headerAddress);
      size_t size = header->size();
      ASSERT(size > 0);
      ASSERT(size < blinkPagePayloadSize());

      if (header->isPromptlyFreed()) {
        ASSERT(size >= sizeof(HeapObjectHeader));
        SET_MEMORY_INACCESSIBLE(headerAddress, sizeof(HeapObjectHeader));
        CHECK_MEMORY_INACCESSIBLE(headerAddress, size);
        freedSize += size;
        headerAddress += size;
        continue;
      }
      if (header->isFree()) {
        SET_MEMORY_INACCESSIBLE(headerAddress, size < sizeof(FreeListEntry)
                                                   ? size
                                                   : sizeof(FreeListEntry));
        CHECK_MEMORY_INACCESSIBLE(headerAddress, size);
         headerAddress += size;
         continue;
       }
      header->checkHeader();
       if (startOfGap != headerAddress)
         addToFreeList(startOfGap, headerAddress - startOfGap);
 
      headerAddress += size;
      startOfGap = headerAddress;
    }

    if (startOfGap != page->payloadEnd())
      addToFreeList(startOfGap, page->payloadEnd() - startOfGap);
  }
  getThreadState()->decreaseAllocatedObjectSize(freedSize);
  ASSERT(m_promptlyFreedSize == freedSize);
  m_promptlyFreedSize = 0;
  return true;
}
