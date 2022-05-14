bool Instance::HandleInputEvent(const pp::InputEvent& event) {
  pp::InputEvent event_device_res(event);
  {
    pp::MouseInputEvent mouse_event(event);
    if (!mouse_event.is_null()) {
      pp::Point point = mouse_event.GetPosition();
      pp::Point movement = mouse_event.GetMovement();
      ScalePoint(device_scale_, &point);
      ScalePoint(device_scale_, &movement);
      mouse_event = pp::MouseInputEvent(
          this,
          event.GetType(),
          event.GetTimeStamp(),
          event.GetModifiers(),
          mouse_event.GetButton(),
          point,
          mouse_event.GetClickCount(),
          movement);
      event_device_res = mouse_event;
    }
  }

  if (event.GetType() == PP_INPUTEVENT_TYPE_MOUSEMOVE &&
     (event.GetModifiers() & PP_INPUTEVENT_MODIFIER_MIDDLEBUTTONDOWN)) {
    pp::MouseInputEvent mouse_event(event_device_res);
    pp::Point pos = mouse_event.GetPosition();
    EnableAutoscroll(pos);
    UpdateCursor(CalculateAutoscroll(pos));
    return true;
  } else {
    DisableAutoscroll();
  }

#ifdef ENABLE_THUMBNAILS
  if (event.GetType() == PP_INPUTEVENT_TYPE_MOUSELEAVE)
    thumbnails_.SlideOut();

  if (thumbnails_.HandleEvent(event_device_res))
    return true;
#endif

  if (toolbar_->HandleEvent(event_device_res))
    return true;

#ifdef ENABLE_THUMBNAILS
  if (v_scrollbar_.get() && event.GetType() == PP_INPUTEVENT_TYPE_MOUSEMOVE) {
    pp::MouseInputEvent mouse_event(event);
    pp::Point pt = mouse_event.GetPosition();
    pp::Rect v_scrollbar_rc;
    v_scrollbar_->GetLocation(&v_scrollbar_rc);
    if (v_scrollbar_rc.Contains(pt) &&
        (event.GetModifiers() & PP_INPUTEVENT_MODIFIER_LEFTBUTTONDOWN)) {
      thumbnails_.SlideIn();
    }

    if (!v_scrollbar_rc.Contains(pt) && thumbnails_.visible() &&
        !(event.GetModifiers() & PP_INPUTEVENT_MODIFIER_LEFTBUTTONDOWN) &&
        !thumbnails_.rect().Contains(pt)) {
      thumbnails_.SlideOut();
    }
  }
#endif

  pp::InputEvent offset_event(event_device_res);
  bool try_engine_first = true;
  switch (offset_event.GetType()) {
    case PP_INPUTEVENT_TYPE_MOUSEDOWN:
    case PP_INPUTEVENT_TYPE_MOUSEUP:
    case PP_INPUTEVENT_TYPE_MOUSEMOVE:
    case PP_INPUTEVENT_TYPE_MOUSEENTER:
    case PP_INPUTEVENT_TYPE_MOUSELEAVE: {
      pp::MouseInputEvent mouse_event(event_device_res);
      pp::MouseInputEvent mouse_event_dip(event);
      pp::Point point = mouse_event.GetPosition();
      point.set_x(point.x() - available_area_.x());
      offset_event = pp::MouseInputEvent(
          this,
          event.GetType(),
          event.GetTimeStamp(),
          event.GetModifiers(),
          mouse_event.GetButton(),
          point,
          mouse_event.GetClickCount(),
          mouse_event.GetMovement());
      if (!engine_->IsSelecting()) {
        if (!IsOverlayScrollbar() &&
            !available_area_.Contains(mouse_event.GetPosition())) {
          try_engine_first = false;
        } else if (IsOverlayScrollbar()) {
          pp::Rect temp;
          if ((v_scrollbar_.get() && v_scrollbar_->GetLocation(&temp) &&
              temp.Contains(mouse_event_dip.GetPosition())) ||
              (h_scrollbar_.get() && h_scrollbar_->GetLocation(&temp) &&
              temp.Contains(mouse_event_dip.GetPosition()))) {
            try_engine_first = false;
          }
        }
      }
      break;
    }
    default:
      break;
  }
  if (try_engine_first && engine_->HandleEvent(offset_event))
    return true;

  if (v_scrollbar_.get() && event.GetType() == PP_INPUTEVENT_TYPE_KEYDOWN) {
    pp::KeyboardInputEvent keyboard_event(event);
    bool no_h_scrollbar = !h_scrollbar_.get();
    uint32_t key_code = keyboard_event.GetKeyCode();
    bool page_down = no_h_scrollbar && key_code == ui::VKEY_RIGHT;
    bool page_up = no_h_scrollbar && key_code == ui::VKEY_LEFT;
    if (zoom_mode_ == ZOOM_FIT_TO_PAGE) {
      bool has_shift =
          keyboard_event.GetModifiers() & PP_INPUTEVENT_MODIFIER_SHIFTKEY;
      bool key_is_space = key_code == ui::VKEY_SPACE;
      page_down |= key_is_space || key_code == ui::VKEY_NEXT;
      page_up |= (key_is_space && has_shift) || (key_code == ui::VKEY_PRIOR);
     }
     if (page_down) {
       int page = engine_->GetFirstVisiblePage();
      if (engine_->GetPageRect(page).bottom() * zoom_ <=
          v_scrollbar_->GetValue())
        page++;
      ScrollToPage(page + 1);
      UpdateCursor(PP_CURSORTYPE_POINTER);
       return true;
     } else if (page_up) {
       int page = engine_->GetFirstVisiblePage();
       if (engine_->GetPageRect(page).y() * zoom_ >= v_scrollbar_->GetValue())
         page--;
       ScrollToPage(page);
      UpdateCursor(PP_CURSORTYPE_POINTER);
      return true;
    }
  }

  if (v_scrollbar_.get() && v_scrollbar_->HandleEvent(event)) {
    UpdateCursor(PP_CURSORTYPE_POINTER);
    return true;
  }

  if (h_scrollbar_.get() && h_scrollbar_->HandleEvent(event)) {
    UpdateCursor(PP_CURSORTYPE_POINTER);
    return true;
  }

  if (timer_pending_ &&
      (event.GetType() == PP_INPUTEVENT_TYPE_MOUSEUP ||
       event.GetType() == PP_INPUTEVENT_TYPE_MOUSEMOVE)) {
    timer_factory_.CancelAll();
    timer_pending_ = false;
  } else if (event.GetType() == PP_INPUTEVENT_TYPE_MOUSEMOVE &&
             engine_->IsSelecting()) {
    bool set_timer = false;
    pp::MouseInputEvent mouse_event(event);
    if (v_scrollbar_.get() &&
        (mouse_event.GetPosition().y() <= 0 ||
         mouse_event.GetPosition().y() >= (plugin_dip_size_.height() - 1))) {
      v_scrollbar_->ScrollBy(
          PP_SCROLLBY_LINE, mouse_event.GetPosition().y() >= 0 ? 1: -1);
      set_timer = true;
    }
    if (h_scrollbar_.get() &&
        (mouse_event.GetPosition().x() <= 0 ||
         mouse_event.GetPosition().x() >= (plugin_dip_size_.width() - 1))) {
      h_scrollbar_->ScrollBy(PP_SCROLLBY_LINE,
          mouse_event.GetPosition().x() >= 0 ? 1: -1);
      set_timer = true;
    }

    if (set_timer) {
      last_mouse_event_ = pp::MouseInputEvent(event);

      pp::CompletionCallback callback =
          timer_factory_.NewCallback(&Instance::OnTimerFired);
      pp::Module::Get()->core()->CallOnMainThread(kDragTimerMs, callback);
      timer_pending_ = true;
    }
  }

  if (event.GetType() == PP_INPUTEVENT_TYPE_KEYDOWN &&
      event.GetModifiers() & kDefaultKeyModifier) {
    pp::KeyboardInputEvent keyboard_event(event);
    switch (keyboard_event.GetKeyCode()) {
      case 'A':
        engine_->SelectAll();
        return true;
    }
  }

  return (event.GetType() == PP_INPUTEVENT_TYPE_MOUSEDOWN);
}
