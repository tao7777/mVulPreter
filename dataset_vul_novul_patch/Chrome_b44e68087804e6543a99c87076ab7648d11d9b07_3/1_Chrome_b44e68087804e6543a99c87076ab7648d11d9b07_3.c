void CoordinatorImpl::RequestGlobalMemoryDumpForPid(
    base::ProcessId pid,
    const RequestGlobalMemoryDumpForPidCallback& callback) {
  if (pid == base::kNullProcessId) {
    callback.Run(false, nullptr);
    return;
  }

  auto adapter = [](const RequestGlobalMemoryDumpForPidCallback& callback,
                    bool success, uint64_t,
                    mojom::GlobalMemoryDumpPtr global_memory_dump) {
    callback.Run(success, std::move(global_memory_dump));
  };

   QueuedRequest::Args args(
       base::trace_event::MemoryDumpType::SUMMARY_ONLY,
       base::trace_event::MemoryDumpLevelOfDetail::BACKGROUND, {},
      false /* addToTrace */, pid);
   RequestGlobalMemoryDumpInternal(args, base::BindRepeating(adapter, callback));
 }
