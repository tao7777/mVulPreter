 void GestureProviderAura::OnGestureEvent(
    const GestureEventData& gesture) {
  GestureEventDetails details = gesture.details;

  if (gesture.type == ET_GESTURE_TAP) {
    int tap_count = 1;
    if (previous_tap_ && IsConsideredDoubleTap(*previous_tap_, gesture))
      tap_count = 1 + (previous_tap_->details.tap_count() % 3);
    details.set_tap_count(tap_count);
    if (!previous_tap_)
      previous_tap_.reset(new GestureEventData(gesture));
    else
      *previous_tap_ = gesture;
    previous_tap_->details = details;
  } else if (gesture.type == ET_GESTURE_TAP_CANCEL) {
    previous_tap_.reset();
  }

  scoped_ptr<ui::GestureEvent> event(
      new ui::GestureEvent(gesture.type,
                           gesture.x,
                           gesture.y,
                           last_touch_event_flags_,
                           gesture.time - base::TimeTicks(),
                           details,
                            1 << gesture.motion_event_id));
 
   if (!handling_event_) {
     client_->OnGestureEvent(event.get());
  } else {
    pending_gestures_.push_back(event.release());
  }
}
