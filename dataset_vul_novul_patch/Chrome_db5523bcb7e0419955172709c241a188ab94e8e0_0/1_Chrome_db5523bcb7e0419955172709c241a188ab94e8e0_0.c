PP_InputEvent_Type ConvertEventTypes(WebInputEvent::Type wetype) {
  switch (wetype) {
    case WebInputEvent::MouseDown:
      return PP_INPUTEVENT_TYPE_MOUSEDOWN;
    case WebInputEvent::MouseUp:
      return PP_INPUTEVENT_TYPE_MOUSEUP;
    case WebInputEvent::MouseMove:
      return PP_INPUTEVENT_TYPE_MOUSEMOVE;
    case WebInputEvent::MouseEnter:
       return PP_INPUTEVENT_TYPE_MOUSEENTER;
     case WebInputEvent::MouseLeave:
       return PP_INPUTEVENT_TYPE_MOUSELEAVE;
     case WebInputEvent::MouseWheel:
       return PP_INPUTEVENT_TYPE_MOUSEWHEEL;
     case WebInputEvent::RawKeyDown:
      return PP_INPUTEVENT_TYPE_RAWKEYDOWN;
    case WebInputEvent::KeyDown:
      return PP_INPUTEVENT_TYPE_KEYDOWN;
    case WebInputEvent::KeyUp:
      return PP_INPUTEVENT_TYPE_KEYUP;
    case WebInputEvent::Char:
      return PP_INPUTEVENT_TYPE_CHAR;
    case WebInputEvent::Undefined:
    default:
      return PP_INPUTEVENT_TYPE_UNDEFINED;
  }
}
