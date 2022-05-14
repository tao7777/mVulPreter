void SecurityHandler::SetRenderer(RenderProcessHost* process_host,
void SecurityHandler::SetRenderer(int process_host_id,
                                   RenderFrameHostImpl* frame_host) {
   host_ = frame_host;
   if (enabled_ && host_)
    AttachToRenderFrameHost();
}
