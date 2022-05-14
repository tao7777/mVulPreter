void FrameSelection::MoveCaretSelection(const IntPoint& point) {
  DCHECK(!GetDocument().NeedsLayoutTreeUpdate());

  Element* const editable =
      ComputeVisibleSelectionInDOMTree().RootEditableElement();
  if (!editable)
    return;

  const VisiblePosition position =
       VisiblePositionForContentsPoint(point, GetFrame());
   SelectionInDOMTree::Builder builder;
   builder.SetIsDirectional(GetSelectionInDOMTree().IsDirectional());
  builder.SetIsHandleVisible(true);
   if (position.IsNotNull())
     builder.Collapse(position.ToPositionWithAffinity());
   SetSelection(builder.Build(), SetSelectionData::Builder()
                                     .SetShouldCloseTyping(true)
                                     .SetShouldClearTypingStyle(true)
                                     .SetSetSelectionBy(SetSelectionBy::kUser)
                                     .Build());
 }
