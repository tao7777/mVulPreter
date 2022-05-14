 void PartialMagnificationController::OnWidgetDestroying(views::Widget* widget) {
  DCHECK_EQ(widget, host_widget_);
   RemoveZoomWidgetObservers();
  host_widget_ = nullptr;
 }
