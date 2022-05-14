void DevToolsSession::SetRenderer(RenderProcessHost* process_host,
void DevToolsSession::SetRenderer(int process_host_id,
                                   RenderFrameHostImpl* frame_host) {
  process_host_id_ = process_host_id;
   host_ = frame_host;
   for (auto& pair : handlers_)
    pair.second->SetRenderer(process_host_id_, host_);
 }
