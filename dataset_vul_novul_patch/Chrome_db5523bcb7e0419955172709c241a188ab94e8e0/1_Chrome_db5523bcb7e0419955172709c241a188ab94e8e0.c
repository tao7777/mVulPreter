WebMouseEvent* BuildMouseEvent(const PP_InputEvent& event) {
  WebMouseEvent* mouse_event = new WebMouseEvent();
  switch (event.type) {
    case PP_INPUTEVENT_TYPE_MOUSEDOWN:
      mouse_event->type = WebInputEvent::MouseDown;
      break;
    case PP_INPUTEVENT_TYPE_MOUSEUP:
      mouse_event->type = WebInputEvent::MouseUp;
      break;
    case PP_INPUTEVENT_TYPE_MOUSEMOVE:
      mouse_event->type = WebInputEvent::MouseMove;
      break;
    case PP_INPUTEVENT_TYPE_MOUSEENTER:
      mouse_event->type = WebInputEvent::MouseEnter;
      break;
     case PP_INPUTEVENT_TYPE_MOUSELEAVE:
       mouse_event->type = WebInputEvent::MouseLeave;
       break;
     default:
       NOTREACHED();
   }
  mouse_event->timeStampSeconds = event.time_stamp;
  mouse_event->modifiers = event.u.mouse.modifier;
  mouse_event->button =
      static_cast<WebMouseEvent::Button>(event.u.mouse.button);
  mouse_event->x = static_cast<int>(event.u.mouse.x);
  mouse_event->y = static_cast<int>(event.u.mouse.y);
  mouse_event->clickCount = event.u.mouse.click_count;
  return mouse_event;
}
