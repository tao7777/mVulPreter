AppCacheUpdateJob::~AppCacheUpdateJob() {
  if (service_)
    service_->RemoveObserver(this);
   if (internal_state_ != COMPLETED)
     Cancel();
 
  DCHECK(!manifest_fetcher_);
  DCHECK(pending_url_fetches_.empty());
   DCHECK(!inprogress_cache_.get());
   DCHECK(pending_master_entries_.empty());
  DCHECK(master_entry_fetches_.empty());
 
   if (group_)
     group_->SetUpdateAppCacheStatus(AppCacheGroup::IDLE);
}
