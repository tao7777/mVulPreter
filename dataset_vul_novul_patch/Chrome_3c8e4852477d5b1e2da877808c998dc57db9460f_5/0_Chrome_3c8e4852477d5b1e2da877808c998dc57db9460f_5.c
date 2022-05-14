void EmulationHandler::SetRenderer(RenderProcessHost* process_host,
void EmulationHandler::SetRenderer(int process_host_id,
                                    RenderFrameHostImpl* frame_host) {
   if (host_ == frame_host)
     return;

  host_ = frame_host;
  if (touch_emulation_enabled_)
    UpdateTouchEventEmulationState();
  UpdateDeviceEmulationState();
}
