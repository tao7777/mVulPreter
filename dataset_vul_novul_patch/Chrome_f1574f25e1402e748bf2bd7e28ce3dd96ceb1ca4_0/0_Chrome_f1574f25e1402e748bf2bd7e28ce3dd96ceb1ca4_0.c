void FrameSelection::FocusedOrActiveStateChanged() {
  bool active_and_focused = FrameIsFocusedAndActive();

  if (Element* element = GetDocument().FocusedElement())
    element->FocusStateChanged();

  GetDocument().UpdateStyleAndLayoutTree();

  auto* view = GetDocument().GetLayoutView();
  if (view)
    layout_selection_->InvalidatePaintForSelection();

  if (active_and_focused)
    SetSelectionFromNone();
  frame_caret_->SetCaretVisibility(active_and_focused
                                       ? CaretVisibility::kVisible
                                       : CaretVisibility::kHidden);
 
   frame_->GetEventHandler().CapsLockStateMayHaveChanged();
 }
