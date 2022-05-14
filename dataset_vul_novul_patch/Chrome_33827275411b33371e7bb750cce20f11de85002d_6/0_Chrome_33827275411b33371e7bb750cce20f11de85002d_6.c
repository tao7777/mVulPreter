bool FrameSelection::SetSelectionDeprecated(
    const SelectionInDOMTree& passed_selection,
    const SetSelectionData& options) {
  DCHECK(IsAvailable());
  passed_selection.AssertValidFor(GetDocument());

  SelectionInDOMTree::Builder builder(passed_selection);
  if (ShouldAlwaysUseDirectionalSelection(frame_))
    builder.SetIsDirectional(true);
  SelectionInDOMTree new_selection = builder.Build();
  if (granularity_strategy_ && !options.DoNotClearStrategy())
    granularity_strategy_->Clear();
  granularity_ = options.Granularity();

  if (options.ShouldCloseTyping())
    TypingCommand::CloseTyping(frame_);

  if (options.ShouldClearTypingStyle())
    frame_->GetEditor().ClearTypingStyle();
 
   const SelectionInDOMTree old_selection_in_dom_tree =
       selection_editor_->GetSelectionInDOMTree();
  const bool is_changed = old_selection_in_dom_tree != new_selection;
  const bool should_show_handle = options.ShouldShowHandle();
  if (!is_changed && is_handle_visible_ == should_show_handle)
     return false;
  if (is_changed)
    selection_editor_->SetSelection(new_selection);
  is_handle_visible_ = should_show_handle;
   ScheduleVisualUpdateForPaintInvalidationIfNeeded();
 
   const Document& current_document = GetDocument();
  frame_->GetEditor().RespondToChangedSelection(
      old_selection_in_dom_tree.ComputeStartPosition(),
      options.ShouldCloseTyping() ? TypingContinuation::kEnd
                                  : TypingContinuation::kContinue);
  DCHECK_EQ(current_document, GetDocument());
  return true;
}
