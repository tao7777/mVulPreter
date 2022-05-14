void RenderWidgetHostViewAura::CreateDelegatedFrameHostClient() {
   if (IsMus())
     return;
 
  cc::FrameSinkId frame_sink_id =
      host_->AllocateFrameSinkId(is_guest_view_hack_);
   if (!delegated_frame_host_client_) {
     delegated_frame_host_client_ =
         base::MakeUnique<DelegatedFrameHostClientAura>(this);
   }
   delegated_frame_host_ = base::MakeUnique<DelegatedFrameHost>(
      frame_sink_id, delegated_frame_host_client_.get());
   if (renderer_compositor_frame_sink_) {
     delegated_frame_host_->DidCreateNewRendererCompositorFrameSink(
         renderer_compositor_frame_sink_);
  }
  UpdateNeedsBeginFramesInternal();

  if (host_->delegate() && host_->delegate()->GetInputEventRouter()) {
    host_->delegate()->GetInputEventRouter()->AddFrameSinkIdOwner(
        GetFrameSinkId(), this);
  }
}
