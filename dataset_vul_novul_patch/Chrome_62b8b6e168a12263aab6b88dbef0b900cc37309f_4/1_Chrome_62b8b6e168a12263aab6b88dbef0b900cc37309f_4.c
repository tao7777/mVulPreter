 void PartialMagnificationController::CloseMagnifierWindow() {
  if (zoom_widget_) {
     RemoveZoomWidgetObservers();
    zoom_widget_->Close();
    zoom_widget_ = NULL;
   }
 }
