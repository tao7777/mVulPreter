WebInputEvent* CreateWebInputEvent(const PP_InputEvent& event) {
  scoped_ptr<WebInputEvent> web_input_event;
  switch (event.type) {
    case PP_INPUTEVENT_TYPE_UNDEFINED:
      return NULL;
    case PP_INPUTEVENT_TYPE_MOUSEDOWN:
    case PP_INPUTEVENT_TYPE_MOUSEUP:
     case PP_INPUTEVENT_TYPE_MOUSEMOVE:
     case PP_INPUTEVENT_TYPE_MOUSEENTER:
     case PP_INPUTEVENT_TYPE_MOUSELEAVE:
       web_input_event.reset(BuildMouseEvent(event));
       break;
     case PP_INPUTEVENT_TYPE_MOUSEWHEEL:
      web_input_event.reset(BuildMouseWheelEvent(event));
      break;
    case PP_INPUTEVENT_TYPE_RAWKEYDOWN:
    case PP_INPUTEVENT_TYPE_KEYDOWN:
    case PP_INPUTEVENT_TYPE_KEYUP:
      web_input_event.reset(BuildKeyEvent(event));
      break;
    case PP_INPUTEVENT_TYPE_CHAR:
      web_input_event.reset(BuildCharEvent(event));
      break;
  }
 
   return web_input_event.release();
 }
