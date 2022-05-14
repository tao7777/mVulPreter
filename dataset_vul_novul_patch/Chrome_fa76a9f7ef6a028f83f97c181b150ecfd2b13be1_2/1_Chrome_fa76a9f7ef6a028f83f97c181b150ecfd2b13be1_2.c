void CoordinatorImpl::PerformNextQueuedGlobalMemoryDump() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  QueuedRequest* request = GetCurrentRequest();

  if (request == nullptr)
    return;

  std::vector<QueuedRequestDispatcher::ClientInfo> clients;
  for (const auto& kv : clients_) {
    auto client_identity = kv.second->identity;
    const base::ProcessId pid = GetProcessIdForClientIdentity(client_identity);
    if (pid == base::kNullProcessId) {
      VLOG(1) << "Couldn't find a PID for client \"" << client_identity.name()
              << "." << client_identity.instance() << "\"";
      continue;
    }
     clients.emplace_back(kv.second->client.get(), pid, kv.second->process_type);
   }
 
  auto chrome_callback = base::Bind(
      &CoordinatorImpl::OnChromeMemoryDumpResponse, base::Unretained(this));
  auto os_callback = base::Bind(&CoordinatorImpl::OnOSMemoryDumpResponse,
                                base::Unretained(this), request->dump_guid);
   QueuedRequestDispatcher::SetUpAndDispatch(request, clients, chrome_callback,
                                             os_callback);
 
   base::SequencedTaskRunnerHandle::Get()->PostDelayedTask(
       FROM_HERE,
       base::BindOnce(&CoordinatorImpl::OnQueuedRequestTimedOut,
                     base::Unretained(this), request->dump_guid),
       client_process_timeout_);
 
   if (request->args.add_to_trace && heap_profiler_) {
    request->heap_dump_in_progress = true;

    bool strip_path_from_mapped_files =
        base::trace_event::TraceLog::GetInstance()
            ->GetCurrentTraceConfig()
             .IsArgumentFilterEnabled();
     heap_profiler_->DumpProcessesForTracing(
         strip_path_from_mapped_files,
            base::BindRepeating(&CoordinatorImpl::OnDumpProcessesForTracing,
                           base::Unretained(this), request->dump_guid));
 
     base::SequencedTaskRunnerHandle::Get()->PostDelayedTask(
         FROM_HERE,
         base::BindOnce(&CoordinatorImpl::OnHeapDumpTimeOut,
                       base::Unretained(this), request->dump_guid),
         kHeapDumpTimeout);
   }
 
  FinalizeGlobalMemoryDumpIfAllManagersReplied();
}
