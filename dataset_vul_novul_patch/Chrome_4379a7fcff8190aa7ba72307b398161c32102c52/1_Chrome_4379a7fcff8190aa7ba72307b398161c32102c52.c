static NavigationPolicy NavigationPolicyForRequest(
    const FrameLoadRequest& request) {
   NavigationPolicy policy = kNavigationPolicyCurrentTab;
  Event* event = request.TriggeringEvent();
  if (!event)
    return policy;
  if (request.Form() && event->UnderlyingEvent())
    event = event->UnderlyingEvent();
   if (event->IsMouseEvent()) {
     MouseEvent* mouse_event = ToMouseEvent(event);
     NavigationPolicyFromMouseEvent(
        mouse_event->button(), mouse_event->ctrlKey(), mouse_event->shiftKey(),
        mouse_event->altKey(), mouse_event->metaKey(), &policy);
  } else if (event->IsKeyboardEvent()) {
    KeyboardEvent* key_event = ToKeyboardEvent(event);
    NavigationPolicyFromMouseEvent(0, key_event->ctrlKey(),
                                   key_event->shiftKey(), key_event->altKey(),
                                   key_event->metaKey(), &policy);
  } else if (event->IsGestureEvent()) {
    GestureEvent* gesture_event = ToGestureEvent(event);
    NavigationPolicyFromMouseEvent(
        0, gesture_event->ctrlKey(), gesture_event->shiftKey(),
        gesture_event->altKey(), gesture_event->metaKey(), &policy);
  }
   return policy;
 }
