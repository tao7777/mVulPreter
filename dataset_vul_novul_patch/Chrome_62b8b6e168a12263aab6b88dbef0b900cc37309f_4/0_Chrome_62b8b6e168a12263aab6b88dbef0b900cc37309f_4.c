 void PartialMagnificationController::CloseMagnifierWindow() {
  if (host_widget_) {
     RemoveZoomWidgetObservers();
    host_widget_->Close();
    host_widget_ = nullptr;
   }
 }
