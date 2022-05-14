CursorImpl::IDBThreadHelper::~IDBThreadHelper() {
  cursor_->RemoveCursorFromTransaction();
}
