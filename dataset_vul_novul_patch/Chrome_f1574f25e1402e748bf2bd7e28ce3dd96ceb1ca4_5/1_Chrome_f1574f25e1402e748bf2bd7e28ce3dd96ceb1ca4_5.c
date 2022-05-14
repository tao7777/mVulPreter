void PasswordInputType::EnableSecureTextInput() {
  LocalFrame* frame = GetElement().GetDocument().GetFrame();
  if (!frame)
    return;
  frame->Selection().SetUseSecureKeyboardEntryWhenActive(true);
}
