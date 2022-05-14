 void CoordinatorImpl::GetVmRegionsForHeapProfiler(
     const GetVmRegionsForHeapProfilerCallback& callback) {
  // This merely strips out the |dump_guid| argument.
  auto adapter = [](const RequestGlobalMemoryDumpCallback& callback,
                    bool success, uint64_t dump_guid,
                    mojom::GlobalMemoryDumpPtr global_memory_dump) {
    callback.Run(success, std::move(global_memory_dump));
  };

  QueuedRequest::Args args(
       MemoryDumpType::EXPLICITLY_TRIGGERED,
      MemoryDumpLevelOfDetail::VM_REGIONS_ONLY_FOR_HEAP_PROFILER, {},
      false /* add_to_trace */, base::kNullProcessId);
  RequestGlobalMemoryDumpInternal(args, base::BindRepeating(adapter, callback));
 }
