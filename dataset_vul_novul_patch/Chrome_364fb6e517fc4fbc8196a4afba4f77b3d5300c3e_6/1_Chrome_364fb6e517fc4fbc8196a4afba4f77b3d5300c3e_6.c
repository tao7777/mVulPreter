void InputMethodLinuxX11::OnCaretBoundsChanged(
    const TextInputClient* client) {
  if (!IsTextInputClientFocused(client))
    return;
  input_method_context_->OnCaretBoundsChanged(
      GetTextInputClient()->GetCaretBounds());
 }
