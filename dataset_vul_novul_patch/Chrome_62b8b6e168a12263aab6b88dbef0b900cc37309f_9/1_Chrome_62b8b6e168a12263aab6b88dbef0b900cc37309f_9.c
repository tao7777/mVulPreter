void PartialMagnificationController::OnMouseMove(
    const gfx::Point& location_in_root) {
  gfx::Point origin(location_in_root);
  origin.Offset(-kMagnifierWidth / 2, -kMagnifierHeight / 2);
 
  if (zoom_widget_) {
    zoom_widget_->SetBounds(
        gfx::Rect(origin.x(), origin.y(), kMagnifierWidth, kMagnifierHeight));
   }
 }
