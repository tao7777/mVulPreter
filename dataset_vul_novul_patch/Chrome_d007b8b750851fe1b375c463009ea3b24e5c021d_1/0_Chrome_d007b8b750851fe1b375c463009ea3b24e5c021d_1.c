 IndexedDBCursor::~IndexedDBCursor() {
  if (transaction_)
    transaction_->UnregisterOpenCursor(this);
   Close();
 }
