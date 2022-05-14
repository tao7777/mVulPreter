 void PartialMagnificationController::OnMouseEvent(ui::MouseEvent* event) {
  if (IsPartialMagnified() && event->type() == ui::ET_MOUSE_MOVED) {
    aura::Window* target = static_cast<aura::Window*>(event->target());
    aura::Window* current_root = target->GetRootWindow();
    gfx::Rect root_bounds = current_root->bounds();
    if (root_bounds.Contains(event->root_location())) {
      SwitchTargetRootWindow(current_root);
      OnMouseMove(event->root_location());
    }
  }
 }
