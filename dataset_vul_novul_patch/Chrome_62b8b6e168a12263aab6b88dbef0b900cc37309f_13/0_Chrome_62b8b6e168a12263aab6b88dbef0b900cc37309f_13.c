 void PartialMagnificationController::RemoveZoomWidgetObservers() {
  DCHECK(host_widget_);
  host_widget_->RemoveObserver(this);
  aura::Window* root_window = host_widget_->GetNativeView()->GetRootWindow();
   DCHECK(root_window);
   root_window->RemoveObserver(this);
 }
