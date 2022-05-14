void CoordinatorImpl::GetVmRegionsForHeapProfiler(
    const std::vector<base::ProcessId>& pids,
    GetVmRegionsForHeapProfilerCallback callback) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  uint64_t dump_guid = ++next_dump_id_;
  std::unique_ptr<QueuedVmRegionRequest> request =
      std::make_unique<QueuedVmRegionRequest>(dump_guid, std::move(callback));
  in_progress_vm_region_requests_[dump_guid] = std::move(request);

  std::vector<QueuedRequestDispatcher::ClientInfo> clients;
  for (const auto& kv : clients_) {
    auto client_identity = kv.second->identity;
    const base::ProcessId pid = GetProcessIdForClientIdentity(client_identity);
    clients.emplace_back(kv.second->client.get(), pid, kv.second->process_type);
  }

  QueuedVmRegionRequest* request_ptr =
       in_progress_vm_region_requests_[dump_guid].get();
   auto os_callback =
       base::BindRepeating(&CoordinatorImpl::OnOSMemoryDumpForVMRegions,
                          weak_ptr_factory_.GetWeakPtr(), dump_guid);
   QueuedRequestDispatcher::SetUpAndDispatchVmRegionRequest(request_ptr, clients,
                                                            pids, os_callback);
   FinalizeVmRegionDumpIfAllManagersReplied(dump_guid);
}
