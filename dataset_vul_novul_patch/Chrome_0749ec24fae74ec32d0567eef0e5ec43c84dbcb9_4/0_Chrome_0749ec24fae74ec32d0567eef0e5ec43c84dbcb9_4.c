Address LargeObjectArena::doAllocateLargeObjectPage(size_t allocationSize,
                                                    size_t gcInfoIndex) {
  size_t largeObjectSize = LargeObjectPage::pageHeaderSize() + allocationSize;
#if defined(ADDRESS_SANITIZER)
  largeObjectSize += allocationGranularity;
#endif

  getThreadState()->shouldFlushHeapDoesNotContainCache();
  PageMemory* pageMemory = PageMemory::allocate(
      largeObjectSize, getThreadState()->heap().getRegionTree());
  Address largeObjectAddress = pageMemory->writableStart();
  Address headerAddress =
      largeObjectAddress + LargeObjectPage::pageHeaderSize();
#if DCHECK_IS_ON()
  for (size_t i = 0; i < largeObjectSize; ++i)
    ASSERT(!largeObjectAddress[i]);
#endif
  ASSERT(gcInfoIndex > 0);
  HeapObjectHeader* header = new (NotNull, headerAddress)
      HeapObjectHeader(largeObjectSizeInHeader, gcInfoIndex);
  Address result = headerAddress + sizeof(*header);
   ASSERT(!(reinterpret_cast<uintptr_t>(result) & allocationMask));
   LargeObjectPage* largeObject = new (largeObjectAddress)
       LargeObjectPage(pageMemory, this, allocationSize);
  header->checkHeader();
 
   ASAN_POISON_MEMORY_REGION(header, sizeof(*header));
  ASAN_POISON_MEMORY_REGION(largeObject->getAddress() + largeObject->size(),
                            allocationGranularity);

  largeObject->link(&m_firstPage);

  getThreadState()->heap().heapStats().increaseAllocatedSpace(
      largeObject->size());
  getThreadState()->increaseAllocatedObjectSize(largeObject->size());
  return result;
}
