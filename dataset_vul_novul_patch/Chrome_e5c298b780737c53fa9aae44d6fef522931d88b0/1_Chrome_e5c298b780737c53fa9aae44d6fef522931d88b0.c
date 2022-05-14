void AppCacheUpdateJob::HandleMasterEntryFetchCompleted(
    URLFetcher* fetcher) {
  DCHECK(internal_state_ == NO_UPDATE || internal_state_ == DOWNLOADING);


  net::URLRequest* request = fetcher->request();
  const GURL& url = request->original_url();
  master_entry_fetches_.erase(url);
  ++master_entries_completed_;

  int response_code = request->status().is_success()
      ? request->GetResponseCode() : -1;

  PendingMasters::iterator found = pending_master_entries_.find(url);
  DCHECK(found != pending_master_entries_.end());
  PendingHosts& hosts = found->second;

  if (response_code / 100 == 2) {
    AppCache* cache = inprogress_cache_.get() ? inprogress_cache_.get()
                                              : group_->newest_complete_cache();
    DCHECK(fetcher->response_writer());
    AppCacheEntry master_entry(AppCacheEntry::MASTER,
                               fetcher->response_writer()->response_id(),
                               fetcher->response_writer()->amount_written());
    if (cache->AddOrModifyEntry(url, master_entry))
      added_master_entries_.push_back(url);
    else
      duplicate_response_ids_.push_back(master_entry.response_id());

    if (!inprogress_cache_.get()) {
      DCHECK(cache == group_->newest_complete_cache());
      for (PendingHosts::iterator host_it = hosts.begin();
           host_it != hosts.end(); ++host_it) {
        (*host_it)->AssociateCompleteCache(cache);
      }
    }
  } else {
    HostNotifier host_notifier;
    for (PendingHosts::iterator host_it = hosts.begin();
         host_it != hosts.end(); ++host_it) {
      AppCacheHost* host = *host_it;
      host_notifier.AddHost(host);

      if (inprogress_cache_.get())
        host->AssociateNoCache(GURL());

      host->RemoveObserver(this);
     }
     hosts.clear();
 
     const char* kFormatString = "Manifest fetch failed (%d) %s";
     std::string message = FormatUrlErrorMessage(
         kFormatString, request->url(), fetcher->result(), response_code);
    host_notifier.SendErrorNotifications(
        AppCacheErrorDetails(message,
                     APPCACHE_MANIFEST_ERROR,
                     request->url(),
                     response_code,
                     false /*is_cross_origin*/));

    if (inprogress_cache_.get()) {
      pending_master_entries_.erase(found);
      --master_entries_completed_;

      if (update_type_ == CACHE_ATTEMPT && pending_master_entries_.empty()) {
        HandleCacheFailure(AppCacheErrorDetails(message,
                                        APPCACHE_MANIFEST_ERROR,
                                        request->url(),
                                        response_code,
                                        false /*is_cross_origin*/),
                           fetcher->result(),
                           GURL());
        return;
      }
    }
  }

  DCHECK(internal_state_ != CACHE_FAILURE);
  FetchMasterEntries();
  MaybeCompleteUpdate();
}
