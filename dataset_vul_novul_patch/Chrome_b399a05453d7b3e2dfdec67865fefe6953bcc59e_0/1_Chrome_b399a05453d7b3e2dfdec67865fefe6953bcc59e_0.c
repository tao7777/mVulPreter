 cc::FrameSinkId RenderWidgetHostViewAura::GetFrameSinkId() {
  return delegated_frame_host_ ? delegated_frame_host_->GetFrameSinkId()
                               : cc::FrameSinkId();
 }
