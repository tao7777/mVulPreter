AppCacheUpdateJob::~AppCacheUpdateJob() {
  if (service_)
    service_->RemoveObserver(this);
   if (internal_state_ != COMPLETED)
     Cancel();
 
   DCHECK(!inprogress_cache_.get());
   DCHECK(pending_master_entries_.empty());

  // The job must not outlive any of its fetchers.
  CHECK(!manifest_fetcher_);
  CHECK(pending_url_fetches_.empty());
  CHECK(master_entry_fetches_.empty());
 
   if (group_)
     group_->SetUpdateAppCacheStatus(AppCacheGroup::IDLE);
}
