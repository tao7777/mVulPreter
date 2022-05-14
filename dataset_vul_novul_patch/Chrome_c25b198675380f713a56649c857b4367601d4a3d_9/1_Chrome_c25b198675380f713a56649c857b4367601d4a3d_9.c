 void MediaControlsProgressView::OnGestureEvent(ui::GestureEvent* event) {
  gfx::Point location_in_bar(event->location());
  ConvertPointToTarget(this, this->progress_bar_, &location_in_bar);
  if (event->type() != ui::ET_GESTURE_TAP ||
      !progress_bar_->GetLocalBounds().Contains(location_in_bar)) {
     return;
   }
 
  HandleSeeking(location_in_bar);
   event->SetHandled();
 }
