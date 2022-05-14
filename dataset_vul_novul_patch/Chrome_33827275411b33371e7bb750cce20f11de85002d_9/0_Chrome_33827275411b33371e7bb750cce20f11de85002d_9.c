void SelectionController::SetNonDirectionalSelectionIfNeeded(
    const SelectionInFlatTree& passed_selection,
    TextGranularity granularity,
    EndPointsAdjustmentMode endpoints_adjustment_mode,
    HandleVisibility handle_visibility) {
  GetDocument().UpdateStyleAndLayoutIgnorePendingStylesheets();

  const VisibleSelectionInFlatTree& new_selection =
      CreateVisibleSelection(passed_selection);
  const PositionInFlatTree& base_position =
      original_base_in_flat_tree_.GetPosition();
  const VisiblePositionInFlatTree& original_base =
      base_position.IsConnected() ? CreateVisiblePosition(base_position)
                                  : VisiblePositionInFlatTree();
  const VisiblePositionInFlatTree& base =
      original_base.IsNotNull() ? original_base
                                : CreateVisiblePosition(new_selection.Base());
  const VisiblePositionInFlatTree& extent =
      CreateVisiblePosition(new_selection.Extent());
  const SelectionInFlatTree& adjusted_selection =
      endpoints_adjustment_mode == kAdjustEndpointsAtBidiBoundary
          ? AdjustEndpointsAtBidiBoundary(base, extent)
          : SelectionInFlatTree::Builder()
                .SetBaseAndExtent(base.DeepEquivalent(),
                                  extent.DeepEquivalent())
                .Build();

  SelectionInFlatTree::Builder builder(new_selection.AsSelection());
  if (adjusted_selection.Base() != base.DeepEquivalent() ||
      adjusted_selection.Extent() != extent.DeepEquivalent()) {
    original_base_in_flat_tree_ = base.ToPositionWithAffinity();
    SetContext(&GetDocument());
    builder.SetBaseAndExtent(adjusted_selection.Base(),
                             adjusted_selection.Extent());
  } else if (original_base.IsNotNull()) {
    if (CreateVisiblePosition(
            Selection().ComputeVisibleSelectionInFlatTree().Base())
            .DeepEquivalent() ==
        CreateVisiblePosition(new_selection.Base()).DeepEquivalent()) {
      builder.SetBaseAndExtent(original_base.DeepEquivalent(),
                               new_selection.Extent());
    }
     original_base_in_flat_tree_ = PositionInFlatTreeWithAffinity();
   }
 
   const SelectionInFlatTree& selection_in_flat_tree = builder.Build();
  const bool should_show_handle =
      handle_visibility == HandleVisibility::kVisible;
   if (Selection().ComputeVisibleSelectionInFlatTree() ==
           CreateVisibleSelection(selection_in_flat_tree) &&
      Selection().IsHandleVisible() == should_show_handle)
     return;
   Selection().SetSelection(
       ConvertToSelectionInDOMTree(selection_in_flat_tree),
      SetSelectionData::Builder()
          .SetShouldCloseTyping(true)
           .SetShouldClearTypingStyle(true)
           .SetCursorAlignOnScroll(CursorAlignOnScroll::kIfNeeded)
           .SetGranularity(granularity)
          .SetShouldShowHandle(should_show_handle)
           .Build());
 }
