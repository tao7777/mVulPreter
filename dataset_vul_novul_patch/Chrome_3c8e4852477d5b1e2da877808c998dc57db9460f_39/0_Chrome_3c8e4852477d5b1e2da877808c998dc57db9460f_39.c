void TargetHandler::SetRenderer(RenderProcessHost* process_host,
void TargetHandler::SetRenderer(int process_host_id,
                                 RenderFrameHostImpl* frame_host) {
   auto_attacher_.SetRenderFrameHost(frame_host);
 }
