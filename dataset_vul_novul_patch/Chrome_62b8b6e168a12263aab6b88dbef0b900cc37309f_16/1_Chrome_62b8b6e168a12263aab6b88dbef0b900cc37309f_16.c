void PartialMagnificationController::SwitchTargetRootWindow(
    aura::Window* new_root_window) {
  if (zoom_widget_ &&
      new_root_window == zoom_widget_->GetNativeView()->GetRootWindow())
    return;
  CloseMagnifierWindow();
  SetScale(GetScale());
}
