 void RenderWidgetHostViewAura::OnLostResources() {
   image_transport_clients_.clear();
   current_surface_ = 0;
  protection_state_id_ = 0;
  current_surface_is_protected_ = true;
  current_surface_in_use_by_compositor_ = true;
  surface_route_id_ = 0;
   UpdateExternalTexture();
   locks_pending_commit_.clear();
 
  DCHECK(!shared_surface_handle_.is_null());
  ImageTransportFactory* factory = ImageTransportFactory::GetInstance();
  factory->DestroySharedSurfaceHandle(shared_surface_handle_);
  shared_surface_handle_ = factory->CreateSharedSurfaceHandle();
  host_->CompositingSurfaceUpdated();
  host_->ScheduleComposite();
}
