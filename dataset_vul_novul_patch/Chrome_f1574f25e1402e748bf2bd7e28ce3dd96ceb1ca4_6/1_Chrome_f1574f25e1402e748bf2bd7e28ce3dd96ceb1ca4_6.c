void Textfield::OnBlur() {
  gfx::RenderText* render_text = GetRenderText();
  render_text->set_focused(false);

  if (PlatformStyle::kTextfieldScrollsToStartOnFocusChange)
    model_->MoveCursorTo(gfx::SelectionModel(0, gfx::CURSOR_FORWARD));

  if (GetInputMethod()) {
    GetInputMethod()->DetachTextInputClient(this);
#if defined(OS_CHROMEOS)
    wm::RestoreWindowBoundsOnClientFocusLost(
        GetNativeView()->GetToplevelWindow());
#endif  // defined(OS_CHROMEOS)
  }
  StopBlinkingCursor();
  cursor_view_.SetVisible(false);

  DestroyTouchSelection();

  if (use_focus_ring_)
     FocusRing::Uninstall(this);
   SchedulePaint();
   View::OnBlur();
 }
