 void InputMethodIMM32::OnCaretBoundsChanged(const TextInputClient* client) {
  if (!enabled_ || !IsTextInputClientFocused(client) ||
      !IsWindowFocused(client)) {
    return;
   }
  DCHECK(!IsTextInputTypeNone());
  gfx::Rect screen_bounds(GetTextInputClient()->GetCaretBounds());
  HWND attached_window = GetAttachedWindowHandle(client);
  RECT r;
  GetClientRect(attached_window, &r);
  POINT window_point = { screen_bounds.x(), screen_bounds.y() };
  ScreenToClient(attached_window, &window_point);
  imm32_manager_.UpdateCaretRect(
      attached_window,
      gfx::Rect(gfx::Point(window_point.x, window_point.y),
                screen_bounds.size()));
 }
