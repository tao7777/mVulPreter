void ServiceWorkerHandler::SetRenderer(RenderProcessHost* process_host,
                                        RenderFrameHostImpl* frame_host) {
  process_ = process_host;
   if (!process_host) {
     ClearForceUpdate();
     context_ = nullptr;
     return;
   }
  StoragePartition* partition = process_host->GetStoragePartition();
  DCHECK(partition);
   context_ = static_cast<ServiceWorkerContextWrapper*>(
      partition->GetServiceWorkerContext());
 }
