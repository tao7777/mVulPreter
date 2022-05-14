 void CoordinatorImpl::RequestGlobalMemoryDumpAndAppendToTrace(
     MemoryDumpType dump_type,
     MemoryDumpLevelOfDetail level_of_detail,
     const RequestGlobalMemoryDumpAndAppendToTraceCallback& callback) {
   auto adapter =
       [](const RequestGlobalMemoryDumpAndAppendToTraceCallback& callback,
         bool success, uint64_t dump_guid,
         mojom::GlobalMemoryDumpPtr) { callback.Run(success, dump_guid); };

  QueuedRequest::Args args(dump_type, level_of_detail, {},
                           true /* add_to_trace */, base::kNullProcessId);
  RequestGlobalMemoryDumpInternal(args, base::BindRepeating(adapter, callback));
}
