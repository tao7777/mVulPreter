 void CoordinatorImpl::RequestGlobalMemoryDump(
     MemoryDumpType dump_type,
     MemoryDumpLevelOfDetail level_of_detail,
     const std::vector<std::string>& allocator_dump_names,
     const RequestGlobalMemoryDumpCallback& callback) {
   auto adapter = [](const RequestGlobalMemoryDumpCallback& callback,
                     bool success, uint64_t,
                    mojom::GlobalMemoryDumpPtr global_memory_dump) {
    callback.Run(success, std::move(global_memory_dump));
  };

  QueuedRequest::Args args(dump_type, level_of_detail, allocator_dump_names,
                           false /* add_to_trace */, base::kNullProcessId);
  RequestGlobalMemoryDumpInternal(args, base::BindRepeating(adapter, callback));
}
