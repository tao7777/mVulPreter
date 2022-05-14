 void InputMethodTSF::OnCaretBoundsChanged(const TextInputClient* client) {
   if (IsTextInputClientFocused(client) && IsWindowFocused(client))
     ui::TSFBridge::GetInstance()->OnTextLayoutChanged();
 }
