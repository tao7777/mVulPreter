void RenderWidgetHostViewAura::AdjustSurfaceProtection() {
  bool surface_is_protected =
      current_surface_ ||
      !host_->is_hidden() ||
      (current_surface_is_protected_ &&
          (pending_thumbnail_tasks_ > 0 ||
              current_surface_in_use_by_compositor_));
  if (current_surface_is_protected_ == surface_is_protected)
    return;
  current_surface_is_protected_ = surface_is_protected;
  ++protection_state_id_;
  if (!surface_route_id_ || !shared_surface_handle_.parent_gpu_process_id)
    return;
  RenderWidgetHostImpl::SendFrontSurfaceIsProtected(
      surface_is_protected,
      protection_state_id_,
      surface_route_id_,
      shared_surface_handle_.parent_gpu_process_id);
 }
