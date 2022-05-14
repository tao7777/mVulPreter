void RenderWidgetHostViewAura::AcceleratedSurfaceRelease(
    uint64 surface_handle) {
  DCHECK(image_transport_clients_.find(surface_handle) !=
         image_transport_clients_.end());
  if (current_surface_ == surface_handle) {
     current_surface_ = 0;
     UpdateExternalTexture();
   }
  image_transport_clients_.erase(surface_handle);
 }
