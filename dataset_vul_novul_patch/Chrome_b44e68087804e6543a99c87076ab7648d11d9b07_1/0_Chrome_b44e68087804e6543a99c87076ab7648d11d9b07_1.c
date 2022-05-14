 void CoordinatorImpl::RequestGlobalMemoryDump(
     MemoryDumpType dump_type,
     MemoryDumpLevelOfDetail level_of_detail,
     const std::vector<std::string>& allocator_dump_names,
     const RequestGlobalMemoryDumpCallback& callback) {
  // Don't allow arbitary processes to obtain VM regions. Only the heap profiler
  // is allowed to obtain them using the special method on the different
  // interface.
  if (level_of_detail ==
      MemoryDumpLevelOfDetail::VM_REGIONS_ONLY_FOR_HEAP_PROFILER) {
    bindings_.ReportBadMessage(
        "Requested global memory dump using level of detail reserved for the "
        "heap profiler.");
    return;
  }

   auto adapter = [](const RequestGlobalMemoryDumpCallback& callback,
                     bool success, uint64_t,
                    mojom::GlobalMemoryDumpPtr global_memory_dump) {
    callback.Run(success, std::move(global_memory_dump));
  };

  QueuedRequest::Args args(dump_type, level_of_detail, allocator_dump_names,
                           false /* add_to_trace */, base::kNullProcessId);
  RequestGlobalMemoryDumpInternal(args, base::BindRepeating(adapter, callback));
}
