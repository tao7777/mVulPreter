void PartialMagnificationController::OnMouseMove(
void PartialMagnificationController::SetActive(bool active) {
  // Fail if we're trying to activate while disabled.
  DCHECK(is_enabled_ || !active);
 
  is_active_ = active;
  if (is_active_) {
    CreateMagnifierWindow(GetCurrentRootWindow());
  } else {
    CloseMagnifierWindow();
   }
 }
