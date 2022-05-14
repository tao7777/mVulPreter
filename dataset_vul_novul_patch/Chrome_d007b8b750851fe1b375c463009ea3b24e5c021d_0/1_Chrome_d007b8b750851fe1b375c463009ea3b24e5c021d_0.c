void IndexedDBCursor::RemoveCursorFromTransaction() {
  if (transaction_)
    transaction_->UnregisterOpenCursor(this);
}
