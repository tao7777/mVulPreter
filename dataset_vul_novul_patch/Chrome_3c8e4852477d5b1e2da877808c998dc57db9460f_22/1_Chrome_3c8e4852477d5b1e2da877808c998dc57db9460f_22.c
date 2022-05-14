void NetworkHandler::SetRenderer(RenderProcessHost* process_host,
                                  RenderFrameHostImpl* frame_host) {
  process_ = process_host;
   host_ = frame_host;
 }
