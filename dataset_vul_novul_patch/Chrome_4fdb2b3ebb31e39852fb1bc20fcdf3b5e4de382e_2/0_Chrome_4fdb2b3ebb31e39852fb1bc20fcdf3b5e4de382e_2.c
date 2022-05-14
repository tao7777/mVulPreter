void ResourceFetcher::PreloadStarted(Resource* resource) {
  if (preloads_ && preloads_->Contains(resource))
    return;
  resource->IncreasePreloadCount();

  if (!preloads_)
     preloads_ = new HeapListHashSet<Member<Resource>>;
   preloads_->insert(resource);
 
  if (preloaded_urls_for_test_)
    preloaded_urls_for_test_->insert(resource->Url().GetString());
 }
