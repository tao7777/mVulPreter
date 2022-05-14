void TargetHandler::SetRenderer(RenderProcessHost* process_host,
                                 RenderFrameHostImpl* frame_host) {
   auto_attacher_.SetRenderFrameHost(frame_host);
 }
