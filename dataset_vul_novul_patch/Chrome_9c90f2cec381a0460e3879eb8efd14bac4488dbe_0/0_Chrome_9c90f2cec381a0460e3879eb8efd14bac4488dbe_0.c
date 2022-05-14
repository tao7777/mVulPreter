bool ClientControlledShellSurface::OnPreWidgetCommit() {
  if (!widget_) {
    if (!pending_geometry_.IsEmpty())
      origin_ = pending_geometry_.origin();
    CreateShellSurfaceWidget(ash::ToWindowShowState(pending_window_state_));
  }

  ash::wm::WindowState* window_state = GetWindowState();
  if (window_state->GetStateType() == pending_window_state_) {
    if (window_state->IsPip() && !window_state->is_dragged()) {
      client_controlled_state_->set_next_bounds_change_animation_type(
          ash::wm::ClientControlledState::kAnimationAnimated);
    }

    return true;
  }

  if (IsPinned(window_state)) {
    VLOG(1) << "State change was requested while pinned";
    return true;
  }

  auto animation_type = ash::wm::ClientControlledState::kAnimationNone;
  switch (pending_window_state_) {
    case ash::WindowStateType::kNormal:
      if (widget_->IsMaximized() || widget_->IsFullscreen()) {
        animation_type = ash::wm::ClientControlledState::kAnimationCrossFade;
      }
      break;

    case ash::WindowStateType::kMaximized:
    case ash::WindowStateType::kFullscreen:
      if (!window_state->IsPip())
        animation_type = ash::wm::ClientControlledState::kAnimationCrossFade;
      break;

    default:
      break;
  }

  if (pending_window_state_ == ash::WindowStateType::kPip) {
    if (ash::features::IsPipRoundedCornersEnabled()) {
      decorator_ = std::make_unique<ash::RoundedCornerDecorator>(
          window_state->window(), host_window(), host_window()->layer(),
          ash::kPipRoundedCornerRadius);
    }
  } else {
    decorator_.reset();  // Remove rounded corners.
  }
 
   bool wasPip = window_state->IsPip();
 
  // As the bounds of the widget is updated later, ensure that no bounds change
  // happens with this state change (e.g. updatePipBounds can be triggered).
  base::AutoReset<bool> resetter(&ignore_bounds_change_request_, true);
   if (client_controlled_state_->EnterNextState(window_state,
                                                pending_window_state_)) {
     client_controlled_state_->set_next_bounds_change_animation_type(
        animation_type);
  }

  if (wasPip && !window_state->IsMinimized()) {
    ash::Shell::Get()->split_view_controller()->EndSplitView(
        ash::SplitViewController::EndReason::kPipExpanded);
    window_state->Activate();
  }

  return true;
}
