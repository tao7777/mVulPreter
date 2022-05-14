void AppCacheHost::SelectCache(const GURL& document_url,
                                const int64 cache_document_was_loaded_from,
                                const GURL& manifest_url) {
   DCHECK(pending_start_update_callback_.is_null() &&
          pending_swap_cache_callback_.is_null() &&
          pending_get_status_callback_.is_null() &&
         !is_selection_pending() && !was_select_cache_called_);
 
   was_select_cache_called_ = true;
   if (!is_cache_selection_enabled_) {
     FinishCacheSelection(NULL, NULL);
    return;
   }
 
   origin_in_use_ = document_url.GetOrigin();
  if (service()->quota_manager_proxy() && !origin_in_use_.is_empty())
    service()->quota_manager_proxy()->NotifyOriginInUse(origin_in_use_);

  if (main_resource_blocked_)
    frontend_->OnContentBlocked(host_id_,
                                blocked_manifest_url_);

 
   if (cache_document_was_loaded_from != kAppCacheNoCacheId) {
     LoadSelectedCache(cache_document_was_loaded_from);
    return;
   }
 
   if (!manifest_url.is_empty() &&
      (manifest_url.GetOrigin() == document_url.GetOrigin())) {
    DCHECK(!first_party_url_.is_empty());
    AppCachePolicy* policy = service()->appcache_policy();
    if (policy &&
        !policy->CanCreateAppCache(manifest_url, first_party_url_)) {
      FinishCacheSelection(NULL, NULL);
      std::vector<int> host_ids(1, host_id_);
      frontend_->OnEventRaised(host_ids, APPCACHE_CHECKING_EVENT);
      frontend_->OnErrorEventRaised(
          host_ids,
          AppCacheErrorDetails(
              "Cache creation was blocked by the content policy",
              APPCACHE_POLICY_ERROR,
              GURL(),
               0,
               false /*is_cross_origin*/));
       frontend_->OnContentBlocked(host_id_, manifest_url);
      return;
     }
 
     set_preferred_manifest_url(manifest_url);
     new_master_entry_url_ = document_url;
     LoadOrCreateGroup(manifest_url);
    return;
   }
 
   FinishCacheSelection(NULL, NULL);
 }
