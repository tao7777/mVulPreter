void OverlayWindowViews::OnGestureEvent(ui::GestureEvent* event) {
  if (event->type() != ui::ET_GESTURE_TAP)
    return;

  hide_controls_timer_.Reset();

  if (!GetControlsScrimLayer()->visible()) {
    UpdateControlsVisibility(true);
    return;
  }

  if (GetCloseControlsBounds().Contains(event->location())) {
    controller_->Close(true /* should_pause_video */,
                       true /* should_reset_pip_player */);
    event->SetHandled();
  } else if (GetPlayPauseControlsBounds().Contains(event->location())) {
     TogglePlayPause();
     event->SetHandled();
   }
  views::Widget::OnGestureEvent(event);
 }
