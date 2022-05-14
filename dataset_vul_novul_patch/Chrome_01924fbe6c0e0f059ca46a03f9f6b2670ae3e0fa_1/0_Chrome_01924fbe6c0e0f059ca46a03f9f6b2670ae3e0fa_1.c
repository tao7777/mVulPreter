 bool GestureProviderAura::OnTouchEvent(const TouchEvent& event) {
   bool pointer_id_is_active = false;
   for (size_t i = 0; i < pointer_state_.GetPointerCount(); ++i) {
     if (event.touch_id() != pointer_state_.GetPointerId(i))
      continue;
    pointer_id_is_active = true;
    break;
  }

  if (event.type() == ET_TOUCH_PRESSED && pointer_id_is_active) {
    return false;
  } else if (event.type() != ET_TOUCH_PRESSED && !pointer_id_is_active) {
     return false;
   }
 
  last_touch_event_flags_ = event.flags();
  last_touch_event_latency_info_ = *event.latency();
   pointer_state_.OnTouch(event);
 
   bool result = filtered_gesture_provider_.OnTouchEvent(pointer_state_);
  pointer_state_.CleanupRemovedTouchPoints(event);
  return result;
}
