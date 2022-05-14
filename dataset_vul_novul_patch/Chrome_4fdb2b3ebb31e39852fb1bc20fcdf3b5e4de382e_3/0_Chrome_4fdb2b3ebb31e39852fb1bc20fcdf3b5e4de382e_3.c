void ResourceFetcher::RequestLoadStarted(unsigned long identifier,
                                         Resource* resource,
                                         const FetchRequest& request,
                                          RevalidationPolicy policy,
                                          bool is_static_data) {
   if (policy == kUse && resource->GetStatus() == ResourceStatus::kCached &&
      !validated_urls_.Contains(resource->Url())) {
     DidLoadResourceFromMemoryCache(identifier, resource,
                                    request.GetResourceRequest());
  }

  if (is_static_data)
     return;
 
   if (policy == kUse && !resource->StillNeedsLoad() &&
      !validated_urls_.Contains(request.GetResourceRequest().Url())) {
     RefPtr<ResourceTimingInfo> info = ResourceTimingInfo::Create(
        request.Options().initiator_info.name, MonotonicallyIncreasingTime(),
        resource->GetType() == Resource::kMainResource);
    PopulateTimingInfo(info.Get(), resource);
    info->ClearLoadTimings();
    info->SetLoadFinishTime(info->InitialTime());
    scheduled_resource_timing_reports_.push_back(info.Release());
    if (!resource_timing_report_timer_.IsActive())
       resource_timing_report_timer_.StartOneShot(0, BLINK_FROM_HERE);
   }
 
  if (validated_urls_.size() >= kMaxValidatedURLsSize) {
    validated_urls_.Clear();
   }
  validated_urls_.insert(request.GetResourceRequest().Url());
 }
