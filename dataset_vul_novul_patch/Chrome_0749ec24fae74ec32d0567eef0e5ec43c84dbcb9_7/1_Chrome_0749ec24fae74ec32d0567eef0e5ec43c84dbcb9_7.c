 static void markPointer(Visitor* visitor, HeapObjectHeader* header) {
  ASSERT(header->checkHeader());
   const GCInfo* gcInfo = ThreadHeap::gcInfo(header->gcInfoIndex());
   if (gcInfo->hasVTable() && !vTableInitialized(header->payload())) {
    visitor->markHeaderNoTracing(header);
    ASSERT(isUninitializedMemory(header->payload(), header->payloadSize()));
  } else {
    visitor->markHeader(header, gcInfo->m_trace);
  }
}
