 void HeapObjectHeader::Finalize(Address object, size_t object_size) {
   HeapAllocHooks::FreeHookIfEnabled(object);
  const GCInfo* gc_info = ThreadHeap::GcInfo(GcInfoIndex());
   if (gc_info->HasFinalizer())
     gc_info->finalize_(object);
 
  ASAN_RETIRE_CONTAINER_ANNOTATION(object, object_size);
}
