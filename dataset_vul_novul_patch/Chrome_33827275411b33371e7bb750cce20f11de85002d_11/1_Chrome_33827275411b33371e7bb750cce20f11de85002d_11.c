void SelectionEditor::NodeWillBeRemoved(Node& node_to_be_removed) {
  if (selection_.IsNone())
    return;
  const Position old_base = selection_.base_;
  const Position old_extent = selection_.extent_;
  const Position& new_base =
      ComputePositionForNodeRemoval(old_base, node_to_be_removed);
  const Position& new_extent =
      ComputePositionForNodeRemoval(old_extent, node_to_be_removed);
  if (new_base == old_base && new_extent == old_extent)
     return;
   selection_ = SelectionInDOMTree::Builder()
                    .SetBaseAndExtent(new_base, new_extent)
                   .SetIsHandleVisible(selection_.IsHandleVisible())
                    .Build();
   MarkCacheDirty();
 }
