void TypingCommand::insertText(Document& document,
void TypingCommand::insertText(
    Document& document,
    const String& text,
    const SelectionInDOMTree& passedSelectionForInsertion,
    Options options,
    TextCompositionType compositionType,
    const bool isIncrementalInsertion) {
   LocalFrame* frame = document.frame();
   DCHECK(frame);
 
   VisibleSelection currentSelection =
       frame->selection().computeVisibleSelectionInDOMTreeDeprecated();
  const VisibleSelection& selectionForInsertion =
      createVisibleSelection(passedSelectionForInsertion);
 
   String newText = text;
   if (compositionType != TextCompositionUpdate)
    newText = dispatchBeforeTextInsertedEvent(text, selectionForInsertion);

  if (compositionType == TextCompositionConfirm) {
    if (dispatchTextInputEvent(frame, newText) !=
        DispatchEventResult::NotCanceled)
      return;
  }

  if (selectionForInsertion.isCaret() && newText.isEmpty())
    return;

  document.updateStyleAndLayoutIgnorePendingStylesheets();

  const PlainTextRange selectionOffsets = getSelectionOffsets(frame);
  if (selectionOffsets.isNull())
    return;
  const size_t selectionStart = selectionOffsets.start();

  if (TypingCommand* lastTypingCommand =
          lastTypingCommandIfStillOpenForTyping(frame)) {
    if (lastTypingCommand->endingSelection() != selectionForInsertion) {
      lastTypingCommand->setStartingSelection(selectionForInsertion);
      lastTypingCommand->setEndingVisibleSelection(selectionForInsertion);
    }

    lastTypingCommand->setCompositionType(compositionType);
    lastTypingCommand->setShouldRetainAutocorrectionIndicator(
        options & RetainAutocorrectionIndicator);
    lastTypingCommand->setShouldPreventSpellChecking(options &
                                                     PreventSpellChecking);
    lastTypingCommand->m_isIncrementalInsertion = isIncrementalInsertion;
    lastTypingCommand->m_selectionStart = selectionStart;

    EditingState editingState;
    EventQueueScope eventQueueScope;
    lastTypingCommand->insertText(newText, options & SelectInsertedText,
                                  &editingState);
    return;
  }

  TypingCommand* command = TypingCommand::create(document, InsertText, newText,
                                                 options, compositionType);
  bool changeSelection = selectionForInsertion != currentSelection;
  if (changeSelection) {
    command->setStartingSelection(selectionForInsertion);
    command->setEndingVisibleSelection(selectionForInsertion);
  }
  command->m_isIncrementalInsertion = isIncrementalInsertion;
  command->m_selectionStart = selectionStart;
  command->apply();

  if (changeSelection) {
    command->setEndingVisibleSelection(currentSelection);
    frame->selection().setSelection(currentSelection.asSelection());
  }
}
