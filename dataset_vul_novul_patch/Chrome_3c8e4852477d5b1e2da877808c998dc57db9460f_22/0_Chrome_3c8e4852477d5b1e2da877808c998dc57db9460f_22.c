void NetworkHandler::SetRenderer(RenderProcessHost* process_host,
void NetworkHandler::SetRenderer(int render_process_host_id,
                                  RenderFrameHostImpl* frame_host) {
  RenderProcessHost* process_host =
      RenderProcessHost::FromID(render_process_host_id);
  if (process_host) {
    storage_partition_ = process_host->GetStoragePartition();
    browser_context_ = process_host->GetBrowserContext();
  } else {
    storage_partition_ = nullptr;
    browser_context_ = nullptr;
  }
   host_ = frame_host;
 }
