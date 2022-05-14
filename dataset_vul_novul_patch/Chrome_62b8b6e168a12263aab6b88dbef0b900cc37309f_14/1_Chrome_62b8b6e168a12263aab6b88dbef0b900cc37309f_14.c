void PartialMagnificationController::SetEnabled(bool enabled) {
  if (enabled) {
    is_enabled_ = enabled;
    SetScale(kDefaultPartialMagnifiedScale);
  } else {
    SetScale(kNonPartialMagnifiedScale);
    is_enabled_ = enabled;
  }
}
