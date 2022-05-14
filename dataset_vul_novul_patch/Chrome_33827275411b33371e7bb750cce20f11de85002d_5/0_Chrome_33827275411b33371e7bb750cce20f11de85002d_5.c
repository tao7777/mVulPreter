void FrameSelection::SelectAll(SetSelectionBy set_selection_by) {
  if (isHTMLSelectElement(GetDocument().FocusedElement())) {
    HTMLSelectElement* select_element =
        toHTMLSelectElement(GetDocument().FocusedElement());
    if (select_element->CanSelectAll()) {
      select_element->SelectAll();
      return;
    }
  }

  Node* root = nullptr;
  Node* select_start_target = nullptr;
  if (set_selection_by == SetSelectionBy::kUser && IsHidden()) {
    root = GetDocument().documentElement();
    select_start_target = GetDocument().body();
  } else if (ComputeVisibleSelectionInDOMTree().IsContentEditable()) {
    root = HighestEditableRoot(ComputeVisibleSelectionInDOMTree().Start());
    if (Node* shadow_root = NonBoundaryShadowTreeRootNode(
            ComputeVisibleSelectionInDOMTree().Start()))
      select_start_target = shadow_root->OwnerShadowHost();
    else
      select_start_target = root;
  } else {
    root = NonBoundaryShadowTreeRootNode(
        ComputeVisibleSelectionInDOMTree().Start());
    if (root) {
      select_start_target = root->OwnerShadowHost();
    } else {
      root = GetDocument().documentElement();
      select_start_target = GetDocument().body();
    }
  }
  if (!root || EditingIgnoresContent(*root))
    return;

  if (select_start_target) {
    const Document& expected_document = GetDocument();
    if (select_start_target->DispatchEvent(Event::CreateCancelableBubble(
            EventTypeNames::selectstart)) != DispatchEventResult::kNotCanceled)
      return;
    if (!IsAvailable()) {
      return;
    }
    if (!root->isConnected() || expected_document != root->GetDocument())
      return;
   }
 
  SetSelection(SelectionInDOMTree::Builder().SelectAllChildren(*root).Build(),
               SetSelectionData::Builder()
                   .SetShouldCloseTyping(true)
                   .SetShouldClearTypingStyle(true)
                   .SetShouldShowHandle(IsHandleVisible())
                    .Build());
   SelectFrameElementInParentIfFullySelected();
  NotifyTextControlOfSelectionChange(SetSelectionBy::kUser);
  if (IsHandleVisible()) {
    ContextMenuAllowedScope scope;
    frame_->GetEventHandler().ShowNonLocatedContextMenu(nullptr,
                                                        kMenuSourceTouch);
  }
}
