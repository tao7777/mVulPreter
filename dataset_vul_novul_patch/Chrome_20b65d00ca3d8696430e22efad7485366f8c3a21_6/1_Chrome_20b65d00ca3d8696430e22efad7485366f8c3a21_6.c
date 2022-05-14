 void MarkingVisitor::ConservativelyMarkHeader(HeapObjectHeader* header) {
  const GCInfo* gc_info = ThreadHeap::GcInfo(header->GcInfoIndex());
   if (gc_info->HasVTable() && !VTableInitialized(header->Payload())) {
    MarkHeaderNoTracing(header);
#if DCHECK_IS_ON()
    DCHECK(IsUninitializedMemory(header->Payload(), header->PayloadSize()));
#endif
  } else {
    MarkHeader(header, gc_info->trace_);
  }
}
