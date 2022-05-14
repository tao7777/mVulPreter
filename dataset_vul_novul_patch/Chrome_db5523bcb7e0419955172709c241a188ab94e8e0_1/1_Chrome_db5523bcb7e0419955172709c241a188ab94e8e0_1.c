void CreatePPEvent(const WebInputEvent& event,
                   std::vector<PP_InputEvent>* pp_events) {
  pp_events->clear();

  switch (event.type) {
    case WebInputEvent::MouseDown:
    case WebInputEvent::MouseUp:
     case WebInputEvent::MouseMove:
     case WebInputEvent::MouseEnter:
     case WebInputEvent::MouseLeave:
       AppendMouseEvent(event, pp_events);
       break;
     case WebInputEvent::MouseWheel:
      AppendMouseWheelEvent(event, pp_events);
      break;
    case WebInputEvent::RawKeyDown:
    case WebInputEvent::KeyDown:
    case WebInputEvent::KeyUp:
      AppendKeyEvent(event, pp_events);
      break;
    case WebInputEvent::Char:
      AppendCharEvent(event, pp_events);
      break;
    case WebInputEvent::Undefined:
    default:
      break;
  }
}
