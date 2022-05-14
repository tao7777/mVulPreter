void DevToolsSession::SetRenderer(RenderProcessHost* process_host,
                                   RenderFrameHostImpl* frame_host) {
  process_ = process_host;
   host_ = frame_host;
   for (auto& pair : handlers_)
    pair.second->SetRenderer(process_, host_);
 }
