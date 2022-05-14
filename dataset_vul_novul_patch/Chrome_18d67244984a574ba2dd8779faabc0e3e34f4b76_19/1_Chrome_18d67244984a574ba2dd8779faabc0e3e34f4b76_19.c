void RenderWidgetHostViewAura::AcceleratedSurfaceBuffersSwapped(
    const GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params& params_in_pixel,
    int gpu_host_id) {
  surface_route_id_ = params_in_pixel.route_id;
  if (params_in_pixel.protection_state_id &&
      params_in_pixel.protection_state_id != protection_state_id_) {
    DCHECK(!current_surface_);
    if (!params_in_pixel.skip_ack)
      InsertSyncPointAndACK(params_in_pixel.route_id, gpu_host_id, false, NULL);
    return;
   }
 
  if (ShouldFastACK(params_in_pixel.surface_handle)) {
    if (!params_in_pixel.skip_ack)
      InsertSyncPointAndACK(params_in_pixel.route_id, gpu_host_id, false, NULL);
    return;
   }
 
  current_surface_ = params_in_pixel.surface_handle;
  if (!params_in_pixel.skip_ack)
    released_front_lock_ = NULL;
 
   UpdateExternalTexture();
 
   ui::Compositor* compositor = GetCompositor();
   if (!compositor) {
    if (!params_in_pixel.skip_ack)
      InsertSyncPointAndACK(params_in_pixel.route_id, gpu_host_id, true, NULL);
   } else {
    DCHECK(image_transport_clients_.find(params_in_pixel.surface_handle) !=
           image_transport_clients_.end());
    gfx::Size surface_size_in_pixel =
        image_transport_clients_[params_in_pixel.surface_handle]->size();
    gfx::Size surface_size = ConvertSizeToDIP(this, surface_size_in_pixel);
    window_->SchedulePaintInRect(gfx::Rect(surface_size));
 
    if (!params_in_pixel.skip_ack) {
      can_lock_compositor_ = NO_PENDING_COMMIT;
      on_compositing_did_commit_callbacks_.push_back(
          base::Bind(&RenderWidgetHostViewAura::InsertSyncPointAndACK,
                     params_in_pixel.route_id,
                     gpu_host_id,
                     true));
      if (!compositor->HasObserver(this))
        compositor->AddObserver(this);
    }
   }
 }
