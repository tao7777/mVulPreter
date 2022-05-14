void InputHandler::SetRenderer(RenderProcessHost* process_host,
                                RenderFrameHostImpl* frame_host) {
   if (frame_host == host_)
     return;
  ClearInputState();
  if (host_) {
    host_->GetRenderWidgetHost()->RemoveInputEventObserver(this);
    if (ignore_input_events_)
      host_->GetRenderWidgetHost()->SetIgnoreInputEvents(false);
  }
  host_ = frame_host;
  if (host_) {
    host_->GetRenderWidgetHost()->AddInputEventObserver(this);
    if (ignore_input_events_)
      host_->GetRenderWidgetHost()->SetIgnoreInputEvents(true);
  }
}
