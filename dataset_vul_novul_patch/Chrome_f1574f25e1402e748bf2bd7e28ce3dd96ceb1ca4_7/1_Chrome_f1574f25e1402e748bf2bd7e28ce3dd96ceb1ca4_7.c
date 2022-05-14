 void Textfield::OnFocus() {
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
