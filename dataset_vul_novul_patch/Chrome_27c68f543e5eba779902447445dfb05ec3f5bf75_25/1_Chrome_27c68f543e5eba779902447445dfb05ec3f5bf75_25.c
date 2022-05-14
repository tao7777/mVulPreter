void Vp9Parser::SetupPastIndependence() {
  memset(&segmentation_, 0, sizeof(segmentation_));
  ResetLoopfilter();
}
