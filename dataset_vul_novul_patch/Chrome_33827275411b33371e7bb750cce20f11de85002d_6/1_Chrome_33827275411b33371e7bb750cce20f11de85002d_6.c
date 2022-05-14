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
  if (old_selection_in_dom_tree == new_selection)
     return false;
  selection_editor_->SetSelection(new_selection);
   ScheduleVisualUpdateForPaintInvalidationIfNeeded();
 
   const Document& current_document = GetDocument();
  frame_->GetEditor().RespondToChangedSelection(
      old_selection_in_dom_tree.ComputeStartPosition(),
      options.ShouldCloseTyping() ? TypingContinuation::kEnd
                                  : TypingContinuation::kContinue);
  DCHECK_EQ(current_document, GetDocument());
  return true;
}
