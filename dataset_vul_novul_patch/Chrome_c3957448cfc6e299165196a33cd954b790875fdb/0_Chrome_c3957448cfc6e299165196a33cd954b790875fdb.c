bool Document::SetFocusedElement(Element* new_focused_element,
                                 const FocusParams& params) {
  DCHECK(!lifecycle_.InDetach());

  clear_focused_element_timer_.Stop();

  if (new_focused_element && (new_focused_element->GetDocument() != this))
    return true;

  if (NodeChildRemovalTracker::IsBeingRemoved(new_focused_element))
    return true;

  if (focused_element_ == new_focused_element)
    return true;

  bool focus_change_blocked = false;
  Element* old_focused_element = focused_element_;
  focused_element_ = nullptr;

  UpdateDistributionForFlatTreeTraversal();
  Node* ancestor = (old_focused_element && old_focused_element->isConnected() &&
                    new_focused_element)
                       ? FlatTreeTraversal::CommonAncestor(*old_focused_element,
                                                           *new_focused_element)
                       : nullptr;

  if (old_focused_element) {
    old_focused_element->SetFocused(false, params.type);
    old_focused_element->SetHasFocusWithinUpToAncestor(false, ancestor);

    if (GetPage() && (GetPage()->GetFocusController().IsFocused())) {
      old_focused_element->DispatchBlurEvent(new_focused_element, params.type,
                                             params.source_capabilities);
      if (focused_element_) {
        focus_change_blocked = true;
        new_focused_element = nullptr;
      }

      old_focused_element->DispatchFocusOutEvent(event_type_names::kFocusout,
                                                 new_focused_element,
                                                 params.source_capabilities);
      old_focused_element->DispatchFocusOutEvent(event_type_names::kDOMFocusOut,
                                                 new_focused_element,
                                                 params.source_capabilities);

      if (focused_element_) {
        focus_change_blocked = true;
        new_focused_element = nullptr;
      }
    }
  }

  if (new_focused_element)
    UpdateStyleAndLayoutTreeForNode(new_focused_element);
  if (new_focused_element && new_focused_element->IsFocusable()) {
     if (IsRootEditableElement(*new_focused_element) &&
         !AcceptsEditingFocus(*new_focused_element)) {
      UpdateStyleAndLayoutTree();
      if (LocalFrame* frame = GetFrame())
        frame->Selection().DidChangeFocus();
      return false;
     }
     focused_element_ = new_focused_element;
    SetSequentialFocusNavigationStartingPoint(focused_element_.Get());

    if (params.type != kWebFocusTypeNone)
      last_focus_type_ = params.type;

    focused_element_->SetFocused(true, params.type);
    focused_element_->SetHasFocusWithinUpToAncestor(true, ancestor);
 
     if (focused_element_ != new_focused_element) {
      UpdateStyleAndLayoutTree();
      if (LocalFrame* frame = GetFrame())
        frame->Selection().DidChangeFocus();
      return false;
     }
     CancelFocusAppearanceUpdate();
     EnsurePaintLocationDataValidForNode(focused_element_);
     focused_element_->UpdateFocusAppearanceWithOptions(
         params.selection_behavior, params.options);
 
    if (GetPage() && (GetPage()->GetFocusController().IsFocused())) {
      focused_element_->DispatchFocusEvent(old_focused_element, params.type,
                                           params.source_capabilities);
 
       if (focused_element_ != new_focused_element) {
        UpdateStyleAndLayoutTree();
        if (LocalFrame* frame = GetFrame())
          frame->Selection().DidChangeFocus();
        return false;
       }
       focused_element_->DispatchFocusInEvent(event_type_names::kFocusin,
                                             old_focused_element, params.type,
                                             params.source_capabilities);
 
       if (focused_element_ != new_focused_element) {
        UpdateStyleAndLayoutTree();
        if (LocalFrame* frame = GetFrame())
          frame->Selection().DidChangeFocus();
        return false;
       }
 
      focused_element_->DispatchFocusInEvent(event_type_names::kDOMFocusIn,
                                             old_focused_element, params.type,
                                             params.source_capabilities);
 
       if (focused_element_ != new_focused_element) {
        UpdateStyleAndLayoutTree();
        if (LocalFrame* frame = GetFrame())
          frame->Selection().DidChangeFocus();
        return false;
       }
     }
   }

  if (!focus_change_blocked && focused_element_) {
    if (AXObjectCache* cache = ExistingAXObjectCache()) {
      cache->HandleFocusedUIElementChanged(old_focused_element,
                                           new_focused_element);
    }
  }

  if (!focus_change_blocked && GetPage()) {
    GetPage()->GetChromeClient().FocusedNodeChanged(old_focused_element,
                                                     focused_element_.Get());
   }
 
   UpdateStyleAndLayoutTree();
   if (LocalFrame* frame = GetFrame())
     frame->Selection().DidChangeFocus();
  return !focus_change_blocked;
}
