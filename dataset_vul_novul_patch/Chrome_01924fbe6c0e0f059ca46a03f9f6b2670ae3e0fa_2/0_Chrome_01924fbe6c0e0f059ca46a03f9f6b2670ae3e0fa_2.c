void GestureProviderAura::OnTouchEventAck(bool event_consumed) {
  DCHECK(pending_gestures_.empty());
   DCHECK(!handling_event_);
   base::AutoReset<bool> handling_event(&handling_event_, true);
   filtered_gesture_provider_.OnTouchEventAck(event_consumed);
  last_touch_event_latency_info_.Clear();
 }
