 void OffscreenCanvasFrameReceiverImpl::SubmitCompositorFrame(
     const cc::SurfaceId& surface_id,
     cc::CompositorFrame frame) {
  if (!surface_factory_) {
    cc::SurfaceManager* manager = GetSurfaceManager();
    surface_factory_ = base::MakeUnique<cc::SurfaceFactory>(manager, this);
    surface_factory_->Create(surface_id);

    GetSurfaceManager()->RegisterSurfaceClientId(surface_id.client_id());
  }
  if (surface_id_.is_null()) {
    surface_id_ = surface_id;
     }
    surface_factory_->SubmitCompositorFrame(surface_id, std::move(frame),
                                            base::Closure());
 }
