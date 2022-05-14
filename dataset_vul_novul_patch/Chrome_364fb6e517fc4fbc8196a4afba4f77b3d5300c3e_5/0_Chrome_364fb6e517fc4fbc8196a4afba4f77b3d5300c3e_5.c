 void InputMethodIMM32::OnCaretBoundsChanged(const TextInputClient* client) {
  if (enabled_ && IsTextInputClientFocused(client) && IsWindowFocused(client)) {
    // The current text input type should not be NONE if |client| is focused.
    DCHECK(!IsTextInputTypeNone());
    gfx::Rect screen_bounds(GetTextInputClient()->GetCaretBounds());

    HWND attached_window = GetAttachedWindowHandle(client);
    // TODO(ime): see comment in TextInputClient::GetCaretBounds(), this
    // conversion shouldn't be necessary.
    RECT r = {};
    GetClientRect(attached_window, &r);
    POINT window_point = { screen_bounds.x(), screen_bounds.y() };
    ScreenToClient(attached_window, &window_point);
    gfx::Rect caret_rect(gfx::Point(window_point.x, window_point.y),
                         screen_bounds.size());
    imm32_manager_.UpdateCaretRect(attached_window, caret_rect);
   }
  InputMethodWin::OnCaretBoundsChanged(client);
 }
