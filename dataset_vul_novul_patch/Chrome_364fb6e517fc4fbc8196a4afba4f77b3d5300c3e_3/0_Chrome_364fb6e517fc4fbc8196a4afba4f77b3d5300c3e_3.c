 void InputMethodIBus::OnCaretBoundsChanged(const TextInputClient* client) {
  OnCaretBoundsChangedInternal(client);
  InputMethodBase::OnCaretBoundsChanged(client);
 }
