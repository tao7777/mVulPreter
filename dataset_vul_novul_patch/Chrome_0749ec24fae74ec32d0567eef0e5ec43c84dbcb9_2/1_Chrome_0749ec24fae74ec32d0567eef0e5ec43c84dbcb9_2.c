bool HeapAllocator::backingShrink(void* address,
                                  size_t quantizedCurrentSize,
                                  size_t quantizedShrunkSize) {
  if (!address || quantizedShrunkSize == quantizedCurrentSize)
    return true;

  ASSERT(quantizedShrunkSize < quantizedCurrentSize);

  ThreadState* state = ThreadState::current();
  if (state->sweepForbidden())
    return false;
  ASSERT(!state->isInGC());
  ASSERT(state->isAllocationAllowed());
  DCHECK_EQ(&state->heap(), &ThreadState::fromObject(address)->heap());

  BasePage* page = pageFromObject(address);
  if (page->isLargeObjectPage() || page->arena()->getThreadState() != state)
     return false;
 
   HeapObjectHeader* header = HeapObjectHeader::fromPayload(address);
  ASSERT(header->checkHeader());
   NormalPageArena* arena = static_cast<NormalPage*>(page)->arenaForNormalPage();
  if (quantizedCurrentSize <=
          quantizedShrunkSize + sizeof(HeapObjectHeader) + sizeof(void*) * 32 &&
      !arena->isObjectAllocatedAtAllocationPoint(header))
    return true;

  bool succeededAtAllocationPoint =
      arena->shrinkObject(header, quantizedShrunkSize);
  if (succeededAtAllocationPoint)
    state->allocationPointAdjusted(arena->arenaIndex());
  return true;
}
