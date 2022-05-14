void SelectionEditor::NodeChildrenWillBeRemoved(ContainerNode& container) {
  if (selection_.IsNone())
    return;
  const Position old_base = selection_.base_;
  const Position old_extent = selection_.extent_;
  const Position& new_base =
      ComputePositionForChildrenRemoval(old_base, container);
  const Position& new_extent =
      ComputePositionForChildrenRemoval(old_extent, container);
  if (new_base == old_base && new_extent == old_extent)
     return;
   selection_ = SelectionInDOMTree::Builder()
                    .SetBaseAndExtent(new_base, new_extent)
                   .SetIsHandleVisible(selection_.IsHandleVisible())
                    .Build();
   MarkCacheDirty();
 }
