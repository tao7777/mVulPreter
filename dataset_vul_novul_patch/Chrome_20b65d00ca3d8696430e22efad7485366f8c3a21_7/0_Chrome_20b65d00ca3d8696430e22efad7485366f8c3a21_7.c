void ProcessHeap::Init() {
  total_allocated_space_ = 0;
   total_allocated_object_size_ = 0;
   total_marked_object_size_ = 0;
 
   base::SamplingHeapProfiler::SetHooksInstallCallback([]() {
     HeapAllocHooks::SetAllocationHook(&BlinkGCAllocHook);
     HeapAllocHooks::SetFreeHook(&BlinkGCFreeHook);
  });
}
