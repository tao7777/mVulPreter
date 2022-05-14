void RenderWidgetHostViewAura::SetSurfaceNotInUseByCompositor(ui::Compositor*) {
  if (current_surface_ || !host_->is_hidden())
    return;
  current_surface_in_use_by_compositor_ = false;
  AdjustSurfaceProtection();
}
