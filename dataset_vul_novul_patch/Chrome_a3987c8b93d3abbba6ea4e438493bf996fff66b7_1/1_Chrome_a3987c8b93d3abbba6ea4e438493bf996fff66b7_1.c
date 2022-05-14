void OffscreenCanvasSurfaceImpl::RequestSurfaceCreation(
    const cc::SurfaceId& surface_id) {
  cc::SurfaceManager* manager = GetSurfaceManager();
  if (!surface_factory_) {
    surface_factory_ = base::MakeUnique<cc::SurfaceFactory>(manager, this);
  }
  surface_factory_->Create(surface_id);
}
