 void InputMethodIBus::OnCaretBoundsChanged(const TextInputClient* client) {
  if (!context_focused_ || !IsTextInputClientFocused(client))
    return;
  DCHECK(!IsTextInputTypeNone());
  const gfx::Rect rect = GetTextInputClient()->GetCaretBounds();
  gfx::Rect composition_head;
  if (!GetTextInputClient()->GetCompositionCharacterBounds(0,
                                                           &composition_head)) {
    composition_head = rect;
  }
  chromeos::IBusPanelCandidateWindowHandlerInterface* candidate_window =
      chromeos::IBusBridge::Get()->GetCandidateWindowHandler();
  if (!candidate_window)
    return;
  candidate_window->SetCursorLocation(
      GfxRectToIBusRect(rect),
      GfxRectToIBusRect(composition_head));
  gfx::Range text_range;
  gfx::Range selection_range;
  string16 surrounding_text;
  if (!GetTextInputClient()->GetTextRange(&text_range) ||
      !GetTextInputClient()->GetTextFromRange(text_range, &surrounding_text) ||
      !GetTextInputClient()->GetSelectionRange(&selection_range)) {
    previous_surrounding_text_.clear();
    previous_selection_range_ = gfx::Range::InvalidRange();
    return;
  }
  if (previous_selection_range_ == selection_range &&
      previous_surrounding_text_ == surrounding_text)
    return;
  previous_selection_range_ = selection_range;
  previous_surrounding_text_ = surrounding_text;
  if (!selection_range.IsValid()) {
    return;
  }
  if (!GetEngine())
    return;
  GetEngine()->SetSurroundingText(
      UTF16ToUTF8(surrounding_text),
      selection_range.start() - text_range.start(),
      selection_range.end() - text_range.start());
 }
