void SecurityHandler::SetRenderer(RenderProcessHost* process_host,
                                   RenderFrameHostImpl* frame_host) {
   host_ = frame_host;
   if (enabled_ && host_)
    AttachToRenderFrameHost();
}
