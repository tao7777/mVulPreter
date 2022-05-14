void ServiceWorkerHandler::SetRenderer(RenderProcessHost* process_host,
void ServiceWorkerHandler::SetRenderer(int process_host_id,
                                        RenderFrameHostImpl* frame_host) {
  RenderProcessHost* process_host = RenderProcessHost::FromID(process_host_id);
   if (!process_host) {
     ClearForceUpdate();
     context_ = nullptr;
     return;
   }

  storage_partition_ =
      static_cast<StoragePartitionImpl*>(process_host->GetStoragePartition());
  DCHECK(storage_partition_);
   context_ = static_cast<ServiceWorkerContextWrapper*>(
      storage_partition_->GetServiceWorkerContext());
 }
