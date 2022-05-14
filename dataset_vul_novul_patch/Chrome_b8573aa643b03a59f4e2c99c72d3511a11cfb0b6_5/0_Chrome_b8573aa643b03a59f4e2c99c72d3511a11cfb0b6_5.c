bool GestureSequence::PinchUpdate(const TouchEvent& event,
     const GesturePoint& point, Gestures* gestures) {
   DCHECK(state_ == GS_PINCH);
   float distance = points_[0].Distance(points_[1]);
  if (abs(distance - pinch_distance_current_) <
      GestureConfiguration::minimum_pinch_update_distance_in_pixels()) {
    if (!points_[0].DidScroll(event,
        GestureConfiguration::minimum_distance_for_pinch_scroll_in_pixels()) ||
        !points_[1].DidScroll(event,
        GestureConfiguration::minimum_distance_for_pinch_scroll_in_pixels()))
       return false;
 
     gfx::Point center = points_[0].last_touch_position().Middle(
                        points_[1].last_touch_position());
    AppendScrollGestureUpdate(point, center, gestures);
  } else {
    AppendPinchGestureUpdate(points_[0], points_[1],
        distance / pinch_distance_current_, gestures);
    pinch_distance_current_ = distance;
  }
  return true;
}
