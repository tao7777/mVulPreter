void PartialMagnificationController::SetScale(float scale) {
void PartialMagnificationController::SetEnabled(bool enabled) {
  is_enabled_ = enabled;
  SetActive(false);
}

void PartialMagnificationController::SwitchTargetRootWindowIfNeeded(
    aura::Window* new_root_window) {
  if (host_widget_ &&
      new_root_window == host_widget_->GetNativeView()->GetRootWindow())
     return;
 
  if (!new_root_window)
    new_root_window = GetCurrentRootWindow();
 
  if (is_enabled_ && is_active_) {
     CloseMagnifierWindow();
    CreateMagnifierWindow(new_root_window);
   }
 }
