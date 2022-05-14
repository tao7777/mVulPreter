 void CoordinatorImpl::RequestGlobalMemoryDumpAndAppendToTrace(
     MemoryDumpType dump_type,
     MemoryDumpLevelOfDetail level_of_detail,
     const RequestGlobalMemoryDumpAndAppendToTraceCallback& callback) {
  // Don't allow arbitary processes to obtain VM regions. Only the heap profiler
  // is allowed to obtain them using the special method on its own dedicated
  // interface (HeapProfilingHelper).
  if (level_of_detail ==
      MemoryDumpLevelOfDetail::VM_REGIONS_ONLY_FOR_HEAP_PROFILER) {
    bindings_.ReportBadMessage(
        "Requested global memory dump using level of detail reserved for the "
        "heap profiler.");
    return;
  }

   auto adapter =
       [](const RequestGlobalMemoryDumpAndAppendToTraceCallback& callback,
         bool success, uint64_t dump_guid,
         mojom::GlobalMemoryDumpPtr) { callback.Run(success, dump_guid); };

  QueuedRequest::Args args(dump_type, level_of_detail, {},
                           true /* add_to_trace */, base::kNullProcessId);
  RequestGlobalMemoryDumpInternal(args, base::BindRepeating(adapter, callback));
}
