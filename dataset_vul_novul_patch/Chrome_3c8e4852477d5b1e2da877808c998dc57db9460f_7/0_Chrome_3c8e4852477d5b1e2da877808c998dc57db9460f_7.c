void InspectorHandler::SetRenderer(RenderProcessHost* process_host,
void InspectorHandler::SetRenderer(int process_host_id,
                                    RenderFrameHostImpl* frame_host) {
   host_ = frame_host;
 }
