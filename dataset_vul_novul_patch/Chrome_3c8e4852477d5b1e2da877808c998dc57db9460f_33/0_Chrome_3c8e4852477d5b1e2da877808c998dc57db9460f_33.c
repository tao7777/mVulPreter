void StorageHandler::SetRenderer(RenderProcessHost* process_host,
void StorageHandler::SetRenderer(int process_host_id,
                                  RenderFrameHostImpl* frame_host) {
  RenderProcessHost* process = RenderProcessHost::FromID(process_host_id);
  storage_partition_ = process ? process->GetStoragePartition() : nullptr;
 }
