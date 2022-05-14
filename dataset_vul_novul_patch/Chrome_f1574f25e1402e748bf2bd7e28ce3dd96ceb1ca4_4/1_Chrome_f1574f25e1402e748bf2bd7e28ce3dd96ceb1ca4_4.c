void PasswordInputType::DisableSecureTextInput() {
  LocalFrame* frame = GetElement().GetDocument().GetFrame();
  if (!frame)
    return;
  frame->Selection().SetUseSecureKeyboardEntryWhenActive(false);
}
