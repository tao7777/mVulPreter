bool PanelBrowserView::OnTitlebarMouseDragged(
    const gfx::Point& mouse_location) {
  if (!mouse_pressed_)
    return false;

  int delta_x = mouse_location.x() - last_mouse_location_.x();
  int delta_y = mouse_location.y() - last_mouse_location_.y();
   if (mouse_dragging_state_ == NO_DRAGGING &&
       ExceededDragThreshold(delta_x, delta_y)) {
    old_focused_view_ = GetFocusManager()->GetFocusedView();
    GetFocusManager()->SetFocusedView(GetFrameView());
 
     panel_->manager()->StartDragging(panel_.get(), last_mouse_location_);
     mouse_dragging_state_ = DRAGGING_STARTED;
  }
  if (mouse_dragging_state_ == DRAGGING_STARTED) {
    panel_->manager()->Drag(mouse_location);

    last_mouse_location_ = mouse_location;
  }
  return true;
}
