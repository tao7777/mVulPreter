void ClientControlledShellSurface::OnBoundsChangeEvent(
    ash::WindowStateType current_state,
    ash::WindowStateType requested_state,
     int64_t display_id,
     const gfx::Rect& window_bounds,
     int bounds_change) {
  if (!geometry().IsEmpty() && !window_bounds.IsEmpty() &&
      (!widget_->IsMinimized() ||
       requested_state != ash::WindowStateType::kMinimized) &&
      bounds_changed_callback_) {
    ash::NonClientFrameViewAsh* frame_view = GetFrameView();

    const bool becoming_snapped =
        requested_state == ash::WindowStateType::kLeftSnapped ||
        requested_state == ash::WindowStateType::kRightSnapped;
    const bool is_tablet_mode =
        WMHelper::GetInstance()->IsTabletModeWindowManagerEnabled();
    gfx::Rect client_bounds =
        becoming_snapped && is_tablet_mode
            ? window_bounds
            : frame_view->GetClientBoundsForWindowBounds(window_bounds);
    gfx::Size current_size = frame_view->GetBoundsForClientView().size();
    bool is_resize = client_bounds.size() != current_size &&
                     !widget_->IsMaximized() && !widget_->IsFullscreen();

    bounds_changed_callback_.Run(current_state, requested_state, display_id,
                                 client_bounds, is_resize, bounds_change);

    auto* window_state = GetWindowState();
    if (server_reparent_window_ &&
        window_state->GetDisplay().id() != display_id) {
      ScopedSetBoundsLocally scoped_set_bounds(this);
      int container_id = window_state->window()->parent()->id();
      aura::Window* new_parent =
          ash::Shell::GetRootWindowControllerWithDisplayId(display_id)
              ->GetContainer(container_id);
      new_parent->AddChild(window_state->window());
    }
  }
}
