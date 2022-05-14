bool PartialMagnificationController::IsPartialMagnified() const {
  return scale_ >= kMinPartialMagnifiedScaleThreshold;
}
