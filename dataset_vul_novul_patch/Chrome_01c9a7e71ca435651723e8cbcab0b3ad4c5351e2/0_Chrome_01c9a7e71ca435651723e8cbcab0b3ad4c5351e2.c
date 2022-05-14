bool PDFiumEngine::HandleEvent(const pp::InputEvent& event) {
  DCHECK(!defer_page_unload_);
  defer_page_unload_ = true;
  bool rv = false;
  switch (event.GetType()) {
    case PP_INPUTEVENT_TYPE_MOUSEDOWN:
      rv = OnMouseDown(pp::MouseInputEvent(event));
      break;
    case PP_INPUTEVENT_TYPE_MOUSEUP:
      rv = OnMouseUp(pp::MouseInputEvent(event));
      break;
    case PP_INPUTEVENT_TYPE_MOUSEMOVE:
      rv = OnMouseMove(pp::MouseInputEvent(event));
      break;
    case PP_INPUTEVENT_TYPE_KEYDOWN:
      rv = OnKeyDown(pp::KeyboardInputEvent(event));
      break;
    case PP_INPUTEVENT_TYPE_KEYUP:
      rv = OnKeyUp(pp::KeyboardInputEvent(event));
      break;
    case PP_INPUTEVENT_TYPE_CHAR:
      rv = OnChar(pp::KeyboardInputEvent(event));
      break;
    case PP_INPUTEVENT_TYPE_TOUCHSTART: {
      KillTouchTimer(next_touch_timer_id_);

      pp::TouchInputEvent touch_event(event);
      if (touch_event.GetTouchCount(PP_TOUCHLIST_TYPE_TARGETTOUCHES) == 1)
        ScheduleTouchTimer(touch_event);
      break;
    }
    case PP_INPUTEVENT_TYPE_TOUCHEND:
      KillTouchTimer(next_touch_timer_id_);
      break;
    case PP_INPUTEVENT_TYPE_TOUCHMOVE:
      KillTouchTimer(next_touch_timer_id_);
    default:
      break;
  }
 
   DCHECK(defer_page_unload_);
   defer_page_unload_ = false;

  // Store the pages to unload away because the act of unloading pages can cause
  // there to be more pages to unload. We leave those extra pages to be unloaded
  // on the next go around.
  std::vector<int> pages_to_unload;
  std::swap(pages_to_unload, deferred_page_unloads_);
  for (int page_index : pages_to_unload)
     pages_[page_index]->Unload();

   return rv;
 }
