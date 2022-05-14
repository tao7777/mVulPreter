void DOMHandler::SetRenderer(RenderProcessHost* process_host,
void DOMHandler::SetRenderer(int process_host_id,
                              RenderFrameHostImpl* frame_host) {
   host_ = frame_host;
 }
