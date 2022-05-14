 void RenderWidgetHostViewAura::AcceleratedSurfacePostSubBuffer(
     const GpuHostMsg_AcceleratedSurfacePostSubBuffer_Params& params_in_pixel,
     int gpu_host_id) {
  surface_route_id_ = params_in_pixel.route_id;
  if (params_in_pixel.protection_state_id &&
      params_in_pixel.protection_state_id != protection_state_id_) {
    DCHECK(!current_surface_);
    InsertSyncPointAndACK(params_in_pixel.route_id, gpu_host_id, false, NULL);
     return;
  }
 
  if (ShouldFastACK(params_in_pixel.surface_handle)) {
    InsertSyncPointAndACK(params_in_pixel.route_id, gpu_host_id, false, NULL);
    return;
   }
 
  current_surface_ = params_in_pixel.surface_handle;
  released_front_lock_ = NULL;
  DCHECK(current_surface_);
  UpdateExternalTexture();
 
  ui::Compositor* compositor = GetCompositor();
  if (!compositor) {
    InsertSyncPointAndACK(params_in_pixel.route_id, gpu_host_id, true, NULL);
  } else {
    DCHECK(image_transport_clients_.find(params_in_pixel.surface_handle) !=
           image_transport_clients_.end());
    gfx::Size surface_size_in_pixel =
        image_transport_clients_[params_in_pixel.surface_handle]->size();
 
     gfx::Rect rect_to_paint = ConvertRectToDIP(this, gfx::Rect(
         params_in_pixel.x,
         surface_size_in_pixel.height() - params_in_pixel.y -
            params_in_pixel.height,
         params_in_pixel.width,
         params_in_pixel.height));
 
    rect_to_paint.Inset(-1, -1);
     rect_to_paint.Intersect(window_->bounds());
 
     window_->SchedulePaintInRect(rect_to_paint);
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
