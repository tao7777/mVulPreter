StorageHandler::GetCacheStorageObserver() {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  if (!cache_storage_observer_) {
     cache_storage_observer_ = std::make_unique<CacheStorageObserver>(
         weak_ptr_factory_.GetWeakPtr(),
         static_cast<CacheStorageContextImpl*>(
            storage_partition_->GetCacheStorageContext()));
   }
   return cache_storage_observer_.get();
 }
