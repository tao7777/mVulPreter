void ThreadHeap::WriteBarrier(void* value) {
  DCHECK(thread_state_->IsIncrementalMarking());
  DCHECK(value);
  DCHECK_NE(value, reinterpret_cast<void*>(-1));

  BasePage* const page = PageFromObject(value);
  HeapObjectHeader* const header =
      page->IsLargeObjectPage()
          ? static_cast<LargeObjectPage*>(page)->GetHeapObjectHeader()
          : static_cast<NormalPage*>(page)->FindHeaderFromAddress(
                reinterpret_cast<Address>(const_cast<void*>(value)));
  if (header->IsMarked())
    return;

   header->Mark();
   marking_worklist_->Push(
       WorklistTaskId::MainThread,
      {header->Payload(),
       GCInfoTable::Get().GCInfoFromIndex(header->GcInfoIndex())->trace_});
 }
