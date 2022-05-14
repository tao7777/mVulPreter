 void CoordinatorImpl::GetVmRegionsForHeapProfiler(
     const GetVmRegionsForHeapProfilerCallback& callback) {
  RequestGlobalMemoryDump(
       MemoryDumpType::EXPLICITLY_TRIGGERED,
      MemoryDumpLevelOfDetail::VM_REGIONS_ONLY_FOR_HEAP_PROFILER, {}, callback);
 }
