void HWNDMessageHandler::Init(HWND parent, const gfx::Rect& bounds) {
  TRACE_EVENT0("views", "HWNDMessageHandler::Init");
  GetMonitorAndRects(bounds.ToRECT(), &last_monitor_, &last_monitor_rect_,
                     &last_work_area_);
 
   WindowImpl::Init(parent, bounds);
 }
