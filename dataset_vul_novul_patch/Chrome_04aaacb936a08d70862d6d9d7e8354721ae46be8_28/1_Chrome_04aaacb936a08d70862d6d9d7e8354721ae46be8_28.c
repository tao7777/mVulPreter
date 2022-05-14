void AppCacheUpdateJob::HandleUrlFetchCompleted(URLFetcher* fetcher,
                                                int net_error) {
  DCHECK(internal_state_ == DOWNLOADING);

  UpdateURLLoaderRequest* request = fetcher->request();
  const GURL& url = request->GetURL();
  pending_url_fetches_.erase(url);
  NotifyAllProgress(url);
  ++url_fetches_completed_;

  int response_code = net_error == net::OK ? request->GetResponseCode()
                                           : fetcher->redirect_response_code();

  AppCacheEntry& entry = url_file_list_.find(url)->second;

  if (response_code / 100 == 2) {
     DCHECK(fetcher->response_writer());
     entry.set_response_id(fetcher->response_writer()->response_id());
    entry.set_response_size(fetcher->response_writer()->amount_written());
     if (!inprogress_cache_->AddOrModifyEntry(url, entry))
       duplicate_response_ids_.push_back(entry.response_id());
 

  } else {
    VLOG(1) << "Request error: " << net_error
            << " response code: " << response_code;
    if (entry.IsExplicit() || entry.IsFallback() || entry.IsIntercept()) {
       if (response_code == 304 && fetcher->existing_entry().has_response_id()) {
         entry.set_response_id(fetcher->existing_entry().response_id());
        entry.set_response_size(fetcher->existing_entry().response_size());
         inprogress_cache_->AddOrModifyEntry(url, entry);
       } else {
         const char kFormatString[] = "Resource fetch failed (%d) %s";
        std::string message = FormatUrlErrorMessage(
            kFormatString, url, fetcher->result(), response_code);
        ResultType result = fetcher->result();
        bool is_cross_origin = url.GetOrigin() != manifest_url_.GetOrigin();
        switch (result) {
          case DISKCACHE_ERROR:
            HandleCacheFailure(
                blink::mojom::AppCacheErrorDetails(
                    message,
                    blink::mojom::AppCacheErrorReason::APPCACHE_UNKNOWN_ERROR,
                    GURL(), 0, is_cross_origin),
                result, url);
            break;
          case NETWORK_ERROR:
            HandleCacheFailure(
                blink::mojom::AppCacheErrorDetails(
                    message,
                    blink::mojom::AppCacheErrorReason::APPCACHE_RESOURCE_ERROR,
                    url, 0, is_cross_origin),
                result, url);
            break;
          default:
            HandleCacheFailure(
                blink::mojom::AppCacheErrorDetails(
                    message,
                    blink::mojom::AppCacheErrorReason::APPCACHE_RESOURCE_ERROR,
                    url, response_code, is_cross_origin),
                result, url);
            break;
        }
        return;
      }
    } else if (response_code == 404 || response_code == 410) {
    } else if (update_type_ == UPGRADE_ATTEMPT &&
               fetcher->existing_entry().has_response_id()) {
       entry.set_response_id(fetcher->existing_entry().response_id());
      entry.set_response_size(fetcher->existing_entry().response_size());
       inprogress_cache_->AddOrModifyEntry(url, entry);
     }
   }

  DCHECK(internal_state_ != CACHE_FAILURE);
  FetchUrls();
  MaybeCompleteUpdate();
}
