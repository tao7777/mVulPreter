StorageHandler::IndexedDBObserver* StorageHandler::GetIndexedDBObserver() {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  if (!indexed_db_observer_) {
     indexed_db_observer_ = std::make_unique<IndexedDBObserver>(
         weak_ptr_factory_.GetWeakPtr(),
         static_cast<IndexedDBContextImpl*>(
            storage_partition_->GetIndexedDBContext()));
   }
   return indexed_db_observer_.get();
 }
