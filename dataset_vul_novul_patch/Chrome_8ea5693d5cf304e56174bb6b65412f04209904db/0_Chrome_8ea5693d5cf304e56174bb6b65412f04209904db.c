void Editor::Transpose() {
//// TODO(yosin): We should move |Transpose()| into |ExecuteTranspose()| in
//// "EditorCommand.cpp"
void Transpose(LocalFrame& frame) {
  Editor& editor = frame.GetEditor();
  if (!editor.CanEdit())
     return;
 
  Document* const document = frame.GetDocument();

  document->UpdateStyleAndLayoutIgnorePendingStylesheets();
 
  const EphemeralRange& range = ComputeRangeForTranspose(frame);
   if (range.IsNull())
     return;
 
  const String& text = PlainText(range);
  if (text.length() != 2)
    return;
   const String& transposed = text.Right(1) + text.Left(1);
 
   if (DispatchBeforeInputInsertText(
          EventTargetNodeForDocument(document), transposed,
           InputEvent::InputType::kInsertTranspose,
           new StaticRangeVector(1, StaticRange::Create(range))) !=
       DispatchEventResult::kNotCanceled)
     return;
 
  // 'beforeinput' event handler may destroy document->
  if (frame.GetDocument() != document)
     return;
 
  document->UpdateStyleAndLayoutIgnorePendingStylesheets();
 
  const EphemeralRange& new_range = ComputeRangeForTranspose(frame);
   if (new_range.IsNull())
     return;
 
  const String& new_text = PlainText(new_range);
  if (new_text.length() != 2)
    return;
  const String& new_transposed = new_text.Right(1) + new_text.Left(1);

  const SelectionInDOMTree& new_selection =
      SelectionInDOMTree::Builder().SetBaseAndExtent(new_range).Build();
 
   if (CreateVisibleSelection(new_selection) !=
      frame.Selection().ComputeVisibleSelectionInDOMTree())
    frame.Selection().SetSelection(new_selection);
 
  editor.ReplaceSelectionWithText(new_transposed, false, false,
                                  InputEvent::InputType::kInsertTranspose);
 }
