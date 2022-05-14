 void MediaControlsProgressView::OnGestureEvent(ui::GestureEvent* event) {
  if (event->type() != ui::ET_GESTURE_TAP || event->y() < kMinClickHeight ||
      event->y() > kMaxClickHeight) {
     return;
   }
 
  HandleSeeking(event->location());
   event->SetHandled();
 }
