IndexedDBTransaction::IndexedDBTransaction(
    int64_t id,
    IndexedDBConnection* connection,
    const std::set<int64_t>& object_store_ids,
    blink::WebIDBTransactionMode mode,
    IndexedDBBackingStore::Transaction* backing_store_transaction)
     : id_(id),
       object_store_ids_(object_store_ids),
       mode_(mode),
      connection_(connection->GetWeakPtr()),
       transaction_(backing_store_transaction),
       ptr_factory_(this) {
   IDB_ASYNC_TRACE_BEGIN("IndexedDBTransaction::lifetime", this);
  callbacks_ = connection_->callbacks();
  database_ = connection_->database();

  diagnostics_.tasks_scheduled = 0;
  diagnostics_.tasks_completed = 0;
  diagnostics_.creation_time = base::Time::Now();
}
