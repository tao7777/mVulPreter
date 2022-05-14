bool HeapAllocator::backingExpand(void* address, size_t newSize) {
  if (!address)
    return false;

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
   bool succeed = arena->expandObject(header, newSize);
   if (succeed)
    state->allocationPointAdjusted(arena->arenaIndex());
  return succeed;
}
