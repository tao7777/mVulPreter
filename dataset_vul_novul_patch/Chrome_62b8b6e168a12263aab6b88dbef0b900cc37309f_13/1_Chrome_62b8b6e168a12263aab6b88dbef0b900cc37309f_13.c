 void PartialMagnificationController::RemoveZoomWidgetObservers() {
  DCHECK(zoom_widget_);
  zoom_widget_->RemoveObserver(this);
  aura::Window* root_window = zoom_widget_->GetNativeView()->GetRootWindow();
   DCHECK(root_window);
   root_window->RemoveObserver(this);
 }
