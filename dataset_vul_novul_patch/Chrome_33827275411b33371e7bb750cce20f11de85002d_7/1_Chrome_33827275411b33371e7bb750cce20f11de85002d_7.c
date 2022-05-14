bool InputMethodController::FinishComposingText(
    ConfirmCompositionBehavior confirm_behavior) {
  if (!HasComposition())
    return false;

  const bool is_too_long = IsTextTooLongAt(composition_range_->StartPosition());

  const String& composing = ComposingText();

  if (confirm_behavior == kKeepSelection) {
    const bool is_handle_visible = GetFrame().Selection().IsHandleVisible();

    const PlainTextRange& old_offsets = GetSelectionOffsets();
    Editor::RevealSelectionScope reveal_selection_scope(&GetEditor());

    if (is_too_long) {
      ReplaceComposition(ComposingText());
    } else {
      Clear();
      DispatchCompositionEndEvent(GetFrame(), composing);
    }

    GetDocument().UpdateStyleAndLayoutIgnorePendingStylesheets();

    const EphemeralRange& old_selection_range =
        EphemeralRangeForOffsets(old_offsets);
    if (old_selection_range.IsNull())
      return false;
     const SelectionInDOMTree& selection =
         SelectionInDOMTree::Builder()
             .SetBaseAndExtent(old_selection_range)
            .SetIsHandleVisible(is_handle_visible)
             .Build();
     GetFrame().Selection().SetSelection(
        selection,
        SetSelectionData::Builder().SetShouldCloseTyping(true).Build());
     return true;
   }
 
  Element* root_editable_element =
      GetFrame()
          .Selection()
          .ComputeVisibleSelectionInDOMTreeDeprecated()
          .RootEditableElement();
  if (!root_editable_element)
    return false;
  PlainTextRange composition_range =
      PlainTextRange::Create(*root_editable_element, *composition_range_);
  if (composition_range.IsNull())
    return false;

  if (is_too_long) {
    ReplaceComposition(ComposingText());
  } else {
    Clear();
  }

  if (!MoveCaret(composition_range.End()))
    return false;

  DispatchCompositionEndEvent(GetFrame(), composing);
  return true;
}
