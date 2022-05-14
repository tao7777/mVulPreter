void DatabaseImpl::IDBThreadHelper::CreateTransaction(
    int64_t transaction_id,
    const std::vector<int64_t>& object_store_ids,
    blink::WebIDBTransactionMode mode) {
  DCHECK(idb_thread_checker_.CalledOnValidThread());
   if (!connection_->IsConnected())
     return;
 
  // Can't call BadMessage as we're no longer on the IO thread. So ignore.
  if (connection_->GetTransaction(transaction_id))
    return;

   connection_->database()->CreateTransaction(transaction_id, connection_.get(),
                                              object_store_ids, mode);
 }
