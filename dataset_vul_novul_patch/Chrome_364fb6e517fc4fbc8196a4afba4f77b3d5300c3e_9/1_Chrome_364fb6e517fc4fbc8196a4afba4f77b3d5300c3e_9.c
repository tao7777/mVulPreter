void InputMethodWin::OnInputLocaleChanged() {
  active_ = imm32_manager_.SetInputLanguage();
   locale_ = imm32_manager_.GetInputLanguageName();
   direction_ = imm32_manager_.GetTextDirection();
   OnInputMethodChanged();
 }
