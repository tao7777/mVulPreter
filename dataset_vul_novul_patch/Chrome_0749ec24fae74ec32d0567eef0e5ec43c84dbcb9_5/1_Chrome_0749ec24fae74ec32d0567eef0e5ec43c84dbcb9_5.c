bool NormalPageArena::expandObject(HeapObjectHeader* header, size_t newSize) {
  ASSERT(header->checkHeader());
   if (header->payloadSize() >= newSize)
     return true;
   size_t allocationSize = ThreadHeap::allocationSizeFromSize(newSize);
  ASSERT(allocationSize > header->size());
  size_t expandSize = allocationSize - header->size();
  if (isObjectAllocatedAtAllocationPoint(header) &&
      expandSize <= m_remainingAllocationSize) {
    m_currentAllocationPoint += expandSize;
    ASSERT(m_remainingAllocationSize >= expandSize);
    setRemainingAllocationSize(m_remainingAllocationSize - expandSize);
    SET_MEMORY_ACCESSIBLE(header->payloadEnd(), expandSize);
    header->setSize(allocationSize);
    ASSERT(findPageFromAddress(header->payloadEnd() - 1));
    return true;
  }
  return false;
 }
