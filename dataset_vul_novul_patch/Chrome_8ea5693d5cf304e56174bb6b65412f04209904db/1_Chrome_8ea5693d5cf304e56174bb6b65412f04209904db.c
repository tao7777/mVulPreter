void Editor::Transpose() {
  if (!CanEdit())
//// TODO(yosin): We should move |Transpose()| into |ExecuteTranspose()| in
//// "EditorCommand.cpp"
     return;
 
  GetFrame().GetDocument()->UpdateStyleAndLayoutIgnorePendingStylesheets();
 
  const EphemeralRange& range = ComputeRangeForTranspose(GetFrame());
   if (range.IsNull())
     return;
 
  const String& text = PlainText(range);
  if (text.length() != 2)
    return;
   const String& transposed = text.Right(1) + text.Left(1);
 
   if (DispatchBeforeInputInsertText(
          EventTargetNodeForDocument(GetFrame().GetDocument()), transposed,
           InputEvent::InputType::kInsertTranspose,
           new StaticRangeVector(1, StaticRange::Create(range))) !=
       DispatchEventResult::kNotCanceled)
     return;
 
  if (frame_->GetDocument()->GetFrame() != frame_)
     return;
 
  GetFrame().GetDocument()->UpdateStyleAndLayoutIgnorePendingStylesheets();
 
  const EphemeralRange& new_range = ComputeRangeForTranspose(GetFrame());
   if (new_range.IsNull())
     return;
 
  const String& new_text = PlainText(new_range);
  if (new_text.length() != 2)
    return;
  const String& new_transposed = new_text.Right(1) + new_text.Left(1);

  const SelectionInDOMTree& new_selection =
      SelectionInDOMTree::Builder().SetBaseAndExtent(new_range).Build();
 
   if (CreateVisibleSelection(new_selection) !=
      GetFrame().Selection().ComputeVisibleSelectionInDOMTree())
    GetFrame().Selection().SetSelection(new_selection);
 
  ReplaceSelectionWithText(new_transposed, false, false,
                           InputEvent::InputType::kInsertTranspose);
 }
