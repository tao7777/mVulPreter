 uint64 Clipboard::GetSequenceNumber(Buffer buffer) {
  if (buffer == BUFFER_STANDARD)
    return SelectionChangeObserver::GetInstance()->clipboard_sequence_number();
  else
    return SelectionChangeObserver::GetInstance()->primary_sequence_number();
 }
