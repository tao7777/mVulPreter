ResourceFetcher::DetermineRevalidationPolicy(Resource::Type type,
                                             const FetchRequest& fetch_request,
                                             Resource* existing_resource,
                                             bool is_static_data) const {
  const ResourceRequest& request = fetch_request.GetResourceRequest();

  if (!existing_resource)
    return kLoad;

  if (existing_resource->Loader() &&
      existing_resource->Loader()->Fetcher() != this) {
    return kReload;
  }

  RecordSriResourceIntegrityMismatchEvent(kCheckingForIntegrityMismatch);
  if (existing_resource->MustRefetchDueToIntegrityMetadata(fetch_request)) {
    RecordSriResourceIntegrityMismatchEvent(kRefetchDueToIntegrityMismatch);
    return kReload;
  }

  if (existing_resource->GetResponse().WasFallbackRequiredByServiceWorker())
    return kReload;

  if (existing_resource->GetType() != type) {
    RESOURCE_LOADING_DVLOG(1) << "ResourceFetcher::determineRevalidationPolicy "
                                 "reloading due to type mismatch.";
    return kReload;
  }

  if (fetch_request.IsSpeculativePreload() && existing_resource->IsPreloaded())
    return kUse;

  if (existing_resource->IsImage() &&
      !Context().AllowImage(images_enabled_, existing_resource->Url())) {
    return kReload;
  }

  if (request.DownloadToFile() || request.UseStreamOnResponse())
    return kReload;

  if (existing_resource->GetResponse().WasFetchedViaServiceWorker() &&
      existing_resource->GetResponse().ServiceWorkerResponseType() ==
          kWebServiceWorkerResponseTypeOpaque &&
      request.GetFetchRequestMode() != WebURLRequest::kFetchRequestModeNoCORS) {
    return kReload;
  }

  if (is_static_data)
    return kUse;

  if (!existing_resource->CanReuse(fetch_request))
    return kReload;

  if (request.IsConditional() ||
      existing_resource->GetResponse().HttpStatusCode() == 304) {
    return kReload;
  }

  if (allow_stale_resources_)
    return kUse;

  if (!fetch_request.Options().CanReuseRequest(existing_resource->Options()))
    return kReload;

  if (existing_resource->IsPreloaded())
    return kUse;

  if (request.GetCachePolicy() == WebCachePolicy::kReturnCacheDataElseLoad)
    return kUse;

  if (existing_resource->HasCacheControlNoStoreHeader()) {
    RESOURCE_LOADING_DVLOG(1) << "ResourceFetcher::determineRevalidationPolicy "
                                 "reloading due to Cache-control: no-store.";
    return kReload;
  }

  if (existing_resource->GetResourceRequest().AllowStoredCredentials() !=
      request.AllowStoredCredentials()) {
    RESOURCE_LOADING_DVLOG(1) << "ResourceFetcher::determineRevalidationPolicy "
                                 "reloading due to difference in credentials "
                                 "settings.";
    return kReload;
  }

   if (type != Resource::kRaw) {
     if (!Context().IsLoadComplete() &&
        validated_urls_.Contains(existing_resource->Url()))
       return kUse;
     if (existing_resource->IsLoading())
       return kUse;
  }

  if (request.GetCachePolicy() == WebCachePolicy::kBypassingCache) {
    RESOURCE_LOADING_DVLOG(1) << "ResourceFetcher::determineRevalidationPolicy "
                                 "reloading due to "
                                 "WebCachePolicy::BypassingCache.";
    return kReload;
  }

  if (existing_resource->ErrorOccurred()) {
    RESOURCE_LOADING_DVLOG(1) << "ResourceFetcher::determineRevalidationPolicy "
                                 "reloading due to resource being in the error "
                                 "state";
    return kReload;
  }

  if (type == Resource::kImage &&
      existing_resource == CachedResource(request.Url())) {
    return kUse;
  }

  if (existing_resource->MustReloadDueToVaryHeader(request))
    return kReload;

  if (!existing_resource->CanReuseRedirectChain()) {
    RESOURCE_LOADING_DVLOG(1) << "ResourceFetcher::determineRevalidationPolicy "
                                 "reloading due to an uncacheable redirect";
    return kReload;
  }

  if (request.GetCachePolicy() == WebCachePolicy::kValidatingCacheData ||
      existing_resource->MustRevalidateDueToCacheHeaders() ||
      request.CacheControlContainsNoCache()) {
    if (existing_resource->CanUseCacheValidator() &&
        !Context().IsControlledByServiceWorker()) {
      if (existing_resource->IsCacheValidator()) {
        DCHECK(existing_resource->StillNeedsLoad());
        return kUse;
      }
      return kRevalidate;
    }

    RESOURCE_LOADING_DVLOG(1) << "ResourceFetcher::determineRevalidationPolicy "
                                 "reloading due to missing cache validators.";
    return kReload;
  }

  return kUse;
}
