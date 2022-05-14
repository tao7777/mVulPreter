void HWNDMessageHandler::Init(HWND parent, const gfx::Rect& bounds) {
  TRACE_EVENT0("views", "HWNDMessageHandler::Init");
  GetMonitorAndRects(bounds.ToRECT(), &last_monitor_, &last_monitor_rect_,
                     &last_work_area_);
 
   WindowImpl::Init(parent, bounds);

#if defined(USE_AURA)
  // Certain trackpad drivers on Windows have bugs where in they don't generate
  // WM_MOUSEWHEEL messages for the trackpoint and trackpad scrolling gestures
  // unless there is an entry for Chrome with the class name of the Window.
  // These drivers check if the window under the trackpoint has the WS_VSCROLL/
  // WS_HSCROLL style and if yes they generate the legacy WM_VSCROLL/WM_HSCROLL
  // messages. We add these styles to ensure that trackpad/trackpoint scrolling
  // work.
  // TODO(ananta)
  // Look into moving the WS_VSCROLL and WS_HSCROLL style setting logic to the
  // CalculateWindowStylesFromInitParams function. Doing it there seems to
  // cause some interactive tests to fail. Investigation needed.
  if (IsTopLevelWindow(hwnd())) {
    long current_style = ::GetWindowLong(hwnd(), GWL_STYLE);
    if (!(current_style & WS_POPUP)) {
      AddScrollStylesToWindow(hwnd());
      needs_scroll_styles_ = true;
    }
  }
#endif
 }
