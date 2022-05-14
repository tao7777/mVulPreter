 void InputMethodBase::OnInputMethodChanged() const {
   TextInputClient* client = GetTextInputClient();
  if (!IsTextInputTypeNone())
     client->OnInputMethodChanged();
 }
