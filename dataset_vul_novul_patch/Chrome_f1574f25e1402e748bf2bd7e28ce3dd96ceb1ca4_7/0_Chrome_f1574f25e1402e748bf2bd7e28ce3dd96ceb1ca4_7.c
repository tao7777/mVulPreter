 void Textfield::OnFocus() {
#if defined(OS_MACOSX)
  if (text_input_type_ == ui::TEXT_INPUT_TYPE_PASSWORD)
    password_input_enabler_.reset(new ui::ScopedPasswordInputEnabler());
#endif  // defined(OS_MACOSX)

   GetRenderText()->set_focused(true);
   if (ShouldShowCursor()) {
     UpdateCursorViewPosition();
    cursor_view_.SetVisible(true);
  }
  if (GetInputMethod())
    GetInputMethod()->SetFocusedTextInputClient(this);
  OnCaretBoundsChanged();
  if (ShouldBlinkCursor())
    StartBlinkingCursor();
  if (use_focus_ring_) {
    FocusRing::Install(this, invalid_
                                 ? ui::NativeTheme::kColorId_AlertSeverityHigh
                                 : ui::NativeTheme::kColorId_NumColors);
  }
  SchedulePaint();
  View::OnFocus();
}
