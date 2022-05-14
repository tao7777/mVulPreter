void Editor::ChangeSelectionAfterCommand(
    const SelectionInDOMTree& new_selection,
    const SetSelectionData& options) {
  if (new_selection.IsNone())
    return;

   bool selection_did_not_change_dom_position =
       new_selection == GetFrame().Selection().GetSelectionInDOMTree();
   GetFrame().Selection().SetSelection(
      new_selection,
      SetSelectionData::Builder(options)
          .SetShouldShowHandle(GetFrame().Selection().IsHandleVisible())
          .Build());
 
  if (selection_did_not_change_dom_position) {
    Client().RespondToChangedSelection(
        frame_, GetFrame().Selection().GetSelectionInDOMTree().Type());
  }
}
