 bool NormalPageArena::shrinkObject(HeapObjectHeader* header, size_t newSize) {
  header->checkHeader();
   ASSERT(header->payloadSize() > newSize);
   size_t allocationSize = ThreadHeap::allocationSizeFromSize(newSize);
   ASSERT(header->size() > allocationSize);
  size_t shrinkSize = header->size() - allocationSize;
  if (isObjectAllocatedAtAllocationPoint(header)) {
    m_currentAllocationPoint -= shrinkSize;
    setRemainingAllocationSize(m_remainingAllocationSize + shrinkSize);
    SET_MEMORY_INACCESSIBLE(m_currentAllocationPoint, shrinkSize);
    header->setSize(allocationSize);
    return true;
  }
  ASSERT(shrinkSize >= sizeof(HeapObjectHeader));
  ASSERT(header->gcInfoIndex() > 0);
  Address shrinkAddress = header->payloadEnd() - shrinkSize;
  HeapObjectHeader* freedHeader = new (NotNull, shrinkAddress)
      HeapObjectHeader(shrinkSize, header->gcInfoIndex());
  freedHeader->markPromptlyFreed();
  ASSERT(pageFromObject(reinterpret_cast<Address>(header)) ==
         findPageFromAddress(reinterpret_cast<Address>(header)));
  m_promptlyFreedSize += shrinkSize;
  header->setSize(allocationSize);
  SET_MEMORY_INACCESSIBLE(shrinkAddress + sizeof(HeapObjectHeader),
                          shrinkSize - sizeof(HeapObjectHeader));
  return false;
}
