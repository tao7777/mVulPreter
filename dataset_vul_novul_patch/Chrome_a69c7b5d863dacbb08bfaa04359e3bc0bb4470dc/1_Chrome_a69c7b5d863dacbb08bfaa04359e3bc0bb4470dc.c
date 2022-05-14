bool Editor::insertTextWithoutSendingTextEvent(const String& text,
                                               bool selectInsertedText,
                                               TextEvent* triggeringEvent) {
  if (text.isEmpty())
    return false;

  const VisibleSelection& selection = selectionForCommand(triggeringEvent);
  if (!selection.isContentEditable())
    return false;

  spellChecker().updateMarkersForWordsAffectedByEditing(
      isSpaceOrNewline(text[0]));
 
   TypingCommand::insertText(
      *selection.start().document(), text, selection,
       selectInsertedText ? TypingCommand::SelectInsertedText : 0,
       triggeringEvent && triggeringEvent->isComposition()
           ? TypingCommand::TextCompositionConfirm
          : TypingCommand::TextCompositionNone);

  if (LocalFrame* editedFrame = selection.start().document()->frame()) {
    if (Page* page = editedFrame->page()) {
      LocalFrame* focusedOrMainFrame =
          toLocalFrame(page->focusController().focusedOrMainFrame());
      focusedOrMainFrame->selection().revealSelection(
          ScrollAlignment::alignCenterIfNeeded);
    }
  }

  return true;
}
