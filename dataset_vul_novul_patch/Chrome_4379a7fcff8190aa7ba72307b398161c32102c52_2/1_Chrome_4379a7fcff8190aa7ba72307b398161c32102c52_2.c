void UpdatePolicyForEvent(const WebInputEvent* input_event,
                          NavigationPolicy* policy) {
  if (!input_event)
    return;

  unsigned short button_number = 0;
  if (input_event->GetType() == WebInputEvent::kMouseUp) {
    const WebMouseEvent* mouse_event =
        static_cast<const WebMouseEvent*>(input_event);

    switch (mouse_event->button) {
      case WebMouseEvent::Button::kLeft:
        button_number = 0;
        break;
      case WebMouseEvent::Button::kMiddle:
        button_number = 1;
        break;
      case WebMouseEvent::Button::kRight:
        button_number = 2;
        break;
      default:
        return;
    }
  } else if ((WebInputEvent::IsKeyboardEventType(input_event->GetType()) &&
              static_cast<const WebKeyboardEvent*>(input_event)
                      ->windows_key_code == VKEY_RETURN) ||
             WebInputEvent::IsGestureEventType(input_event->GetType())) {
    button_number = 0;
  } else {
    return;
  }

  bool ctrl = input_event->GetModifiers() & WebInputEvent::kControlKey;
  bool shift = input_event->GetModifiers() & WebInputEvent::kShiftKey;
  bool alt = input_event->GetModifiers() & WebInputEvent::kAltKey;
  bool meta = input_event->GetModifiers() & WebInputEvent::kMetaKey;

  NavigationPolicy user_policy = *policy;
   NavigationPolicyFromMouseEvent(button_number, ctrl, shift, alt, meta,
                                  &user_policy);
 
  if (user_policy == kNavigationPolicyDownload &&
      *policy != kNavigationPolicyIgnore)
    return;
   if (user_policy == kNavigationPolicyNewWindow &&
      *policy == kNavigationPolicyNewPopup)
    return;
   *policy = user_policy;
 }
