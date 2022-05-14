void RenderWidgetHostImpl::DidNavigate(uint32_t next_source_id) {
  current_content_source_id_ = next_source_id;
  did_receive_first_frame_after_navigation_ = false;

  if (enable_surface_synchronization_) {
     visual_properties_ack_pending_ = false;
    viz::LocalSurfaceId old_surface_id = view_->GetLocalSurfaceId();
     if (view_)
       view_->DidNavigate();
    viz::LocalSurfaceId new_surface_id = view_->GetLocalSurfaceId();
    if (old_surface_id == new_surface_id)
      return;
   } else {
    if (last_received_content_source_id_ >= current_content_source_id_)
      return;
  }

  if (!new_content_rendering_timeout_)
    return;

  new_content_rendering_timeout_->Start(new_content_rendering_delay_);
}
