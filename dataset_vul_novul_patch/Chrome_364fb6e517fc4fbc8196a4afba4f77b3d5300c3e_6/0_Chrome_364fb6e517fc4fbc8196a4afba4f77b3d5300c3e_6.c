void InputMethodLinuxX11::OnCaretBoundsChanged(
void InputMethodLinuxX11::OnCaretBoundsChanged(const TextInputClient* client) {
  if (IsTextInputClientFocused(client)) {
    input_method_context_->OnCaretBoundsChanged(
        GetTextInputClient()->GetCaretBounds());
  }
  InputMethodBase::OnCaretBoundsChanged(client);
 }
