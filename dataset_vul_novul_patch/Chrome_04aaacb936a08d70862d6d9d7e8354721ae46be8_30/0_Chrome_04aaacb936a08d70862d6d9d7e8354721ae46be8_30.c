void AppCacheUpdateJob::OnResponseInfoLoaded(
    AppCacheResponseInfo* response_info,
    int64_t response_id) {
  const net::HttpResponseInfo* http_info =
      response_info ? &response_info->http_response_info() : nullptr;

  if (internal_state_ == FETCH_MANIFEST) {
    if (http_info)
      manifest_fetcher_->set_existing_response_headers(
          http_info->headers.get());
    manifest_fetcher_->Start();
    return;
  }

  auto found = loading_responses_.find(response_id);
  DCHECK(found != loading_responses_.end());
  const GURL& url = found->second;

  if (!http_info) {
    LoadFromNewestCacheFailed(url, nullptr);  // no response found
  } else if (!CanUseExistingResource(http_info)) {
    LoadFromNewestCacheFailed(url, response_info);
  } else {
    DCHECK(group_->newest_complete_cache());
    AppCacheEntry* copy_me = group_->newest_complete_cache()->GetEntry(url);
    DCHECK(copy_me);
    DCHECK_EQ(copy_me->response_id(), response_id);

    auto it = url_file_list_.find(url);
     DCHECK(it != url_file_list_.end());
     AppCacheEntry& entry = it->second;
     entry.set_response_id(response_id);
    entry.SetResponseAndPaddingSizes(copy_me->response_size(),
                                     copy_me->padding_size());
     inprogress_cache_->AddOrModifyEntry(url, entry);
     NotifyAllProgress(url);
     ++url_fetches_completed_;
  }

  loading_responses_.erase(found);
  MaybeCompleteUpdate();
}
