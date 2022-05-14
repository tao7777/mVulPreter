 void RenderWidgetHostViewAura::OnLostResources() {
   image_transport_clients_.clear();
   current_surface_ = 0;
   UpdateExternalTexture();
   locks_pending_commit_.clear();
 
  DCHECK(!shared_surface_handle_.is_null());
  ImageTransportFactory* factory = ImageTransportFactory::GetInstance();
  factory->DestroySharedSurfaceHandle(shared_surface_handle_);
  shared_surface_handle_ = factory->CreateSharedSurfaceHandle();
  host_->CompositingSurfaceUpdated();
  host_->ScheduleComposite();
}
