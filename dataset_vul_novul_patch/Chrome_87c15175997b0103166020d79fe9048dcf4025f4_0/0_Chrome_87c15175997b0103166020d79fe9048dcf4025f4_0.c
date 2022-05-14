 gfx::Vector2d GetMouseWheelOffset(const base::NativeEvent& native_event) {
  DCHECK(native_event.message == WM_MOUSEWHEEL ||
         native_event.message == WM_MOUSEHWHEEL);
  if (native_event.message == WM_MOUSEWHEEL)
    return gfx::Vector2d(0, GET_WHEEL_DELTA_WPARAM(native_event.wParam));
  return gfx::Vector2d(GET_WHEEL_DELTA_WPARAM(native_event.wParam), 0);
 }
