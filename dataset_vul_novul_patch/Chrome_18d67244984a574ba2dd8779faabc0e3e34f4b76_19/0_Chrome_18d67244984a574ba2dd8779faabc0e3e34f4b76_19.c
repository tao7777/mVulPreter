void RenderWidgetHostViewAura::AcceleratedSurfaceBuffersSwapped(
bool RenderWidgetHostViewAura::SwapBuffersPrepare(
    const gfx::Rect& surface_rect,
    const gfx::Rect& damage_rect,
    BufferPresentedParams* params) {
  DCHECK(params->surface_handle);
  DCHECK(!params->texture_to_produce);

  if (last_swapped_surface_size_ != surface_rect.size()) {
    // The surface could have shrunk since we skipped an update, in which
    // case we can expect a full update.
    DLOG_IF(ERROR, damage_rect != surface_rect) << "Expected full damage rect";
    skipped_damage_.setEmpty();
    last_swapped_surface_size_ = surface_rect.size();
   }
 
  if (ShouldSkipFrame(surface_rect.size())) {
    skipped_damage_.op(RectToSkIRect(damage_rect), SkRegion::kUnion_Op);
    InsertSyncPointAndACK(*params);
    return false;
   }
 
  DCHECK(!current_surface_ || image_transport_clients_.find(current_surface_) !=
      image_transport_clients_.end());
  if (current_surface_)
    params->texture_to_produce = image_transport_clients_[current_surface_];

  std::swap(current_surface_, params->surface_handle);
 
  DCHECK(image_transport_clients_.find(current_surface_) !=
      image_transport_clients_.end());

  image_transport_clients_[current_surface_]->Consume(surface_rect.size());
  released_front_lock_ = NULL;
   UpdateExternalTexture();
 
  return true;
}

void RenderWidgetHostViewAura::SwapBuffersCompleted(
    const BufferPresentedParams& params) {
   ui::Compositor* compositor = GetCompositor();
   if (!compositor) {
    InsertSyncPointAndACK(params);
   } else {
    // Add sending an ACK to the list of things to do OnCompositingDidCommit
    can_lock_compositor_ = NO_PENDING_COMMIT;
    on_compositing_did_commit_callbacks_.push_back(
        base::Bind(&RenderWidgetHostViewAura::InsertSyncPointAndACK, params));
    if (!compositor->HasObserver(this))
      compositor->AddObserver(this);
  }
}
 
void RenderWidgetHostViewAura::AcceleratedSurfaceBuffersSwapped(
    const GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params& params_in_pixel,
    int gpu_host_id) {
  const gfx::Rect surface_rect = gfx::Rect(gfx::Point(), params_in_pixel.size);
  BufferPresentedParams ack_params(
      params_in_pixel.route_id, gpu_host_id, params_in_pixel.surface_handle);
  if (!SwapBuffersPrepare(surface_rect, surface_rect, &ack_params))
    return;

  previous_damage_.setRect(RectToSkIRect(surface_rect));
  skipped_damage_.setEmpty();

  ui::Compositor* compositor = GetCompositor();
  if (compositor) {
    gfx::Size surface_size = ConvertSizeToDIP(this, params_in_pixel.size);
    window_->SchedulePaintInRect(gfx::Rect(surface_size));
   }

  SwapBuffersCompleted(ack_params);
 }
