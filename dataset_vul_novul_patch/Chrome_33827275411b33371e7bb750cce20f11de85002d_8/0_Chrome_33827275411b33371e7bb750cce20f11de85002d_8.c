SelectionInDOMTree ConvertToSelectionInDOMTree(
    const SelectionInFlatTree& selection_in_flat_tree) {
  return SelectionInDOMTree::Builder()
      .SetAffinity(selection_in_flat_tree.Affinity())
       .SetBaseAndExtent(ToPositionInDOMTree(selection_in_flat_tree.Base()),
                         ToPositionInDOMTree(selection_in_flat_tree.Extent()))
       .SetIsDirectional(selection_in_flat_tree.IsDirectional())
       .Build();
 }
