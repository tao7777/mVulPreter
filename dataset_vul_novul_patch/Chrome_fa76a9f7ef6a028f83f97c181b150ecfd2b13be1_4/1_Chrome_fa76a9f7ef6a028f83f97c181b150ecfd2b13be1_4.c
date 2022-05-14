void CoordinatorImpl::UnregisterClientProcess(
    mojom::ClientProcess* client_process) {
  QueuedRequest* request = GetCurrentRequest();
  if (request != nullptr) {
    auto it = request->pending_responses.begin();
    while (it != request->pending_responses.end()) {
      std::set<QueuedRequest::PendingResponse>::iterator current = it++;
      if (current->client != client_process)
        continue;
      RemovePendingResponse(client_process, current->type);
      request->failed_memory_dump_count++;
    }
    FinalizeGlobalMemoryDumpIfAllManagersReplied();
  }

  for (auto& pair : in_progress_vm_region_requests_) {
    QueuedVmRegionRequest* request = pair.second.get();
    auto it = request->pending_responses.begin();
    while (it != request->pending_responses.end()) {
      auto current = it++;
      if (*current == client_process) {
        request->pending_responses.erase(current);
      }
    }
  }

  for (auto& pair : in_progress_vm_region_requests_) {
    base::SequencedTaskRunnerHandle::Get()->PostTask(
         FROM_HERE,
         base::BindOnce(
             &CoordinatorImpl::FinalizeVmRegionDumpIfAllManagersReplied,
            base::Unretained(this), pair.second->dump_guid));
   }
 
   size_t num_deleted = clients_.erase(client_process);
  DCHECK(num_deleted == 1);
}
