 void NormalPageArena::promptlyFreeObject(HeapObjectHeader* header) {
   ASSERT(!getThreadState()->sweepForbidden());
  ASSERT(header->checkHeader());
   Address address = reinterpret_cast<Address>(header);
   Address payload = header->payload();
   size_t size = header->size();
  size_t payloadSize = header->payloadSize();
  ASSERT(size > 0);
  ASSERT(pageFromObject(address) == findPageFromAddress(address));

  {
    ThreadState::SweepForbiddenScope forbiddenScope(getThreadState());
    header->finalize(payload, payloadSize);
    if (address + size == m_currentAllocationPoint) {
      m_currentAllocationPoint = address;
      setRemainingAllocationSize(m_remainingAllocationSize + size);
      SET_MEMORY_INACCESSIBLE(address, size);
      return;
    }
    SET_MEMORY_INACCESSIBLE(payload, payloadSize);
    header->markPromptlyFreed();
  }

  m_promptlyFreedSize += size;
}
