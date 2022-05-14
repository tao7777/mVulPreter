void IOHandler::SetRenderer(RenderProcessHost* process_host,
void IOHandler::SetRenderer(int process_host_id,
                             RenderFrameHostImpl* frame_host) {
  RenderProcessHost* process_host = RenderProcessHost::FromID(process_host_id);
  if (process_host) {
    browser_context_ = process_host->GetBrowserContext();
    storage_partition_ = process_host->GetStoragePartition();
  } else {
    browser_context_ = nullptr;
    storage_partition_ = nullptr;
  }
 }
