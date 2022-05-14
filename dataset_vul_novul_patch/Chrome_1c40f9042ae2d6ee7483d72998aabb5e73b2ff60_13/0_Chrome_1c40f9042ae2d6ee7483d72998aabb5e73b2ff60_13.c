bool ResourceFetcher::StartLoad(Resource* resource) {
  DCHECK(resource);
  DCHECK(resource->StillNeedsLoad());

  ResourceRequest request(resource->GetResourceRequest());
  ResourceLoader* loader = nullptr;

  {
    Resource::RevalidationStartForbiddenScope
        revalidation_start_forbidden_scope(resource);
    ScriptForbiddenIfMainThreadScope script_forbidden_scope;

    if (!Context().ShouldLoadNewResource(resource->GetType()) &&
        IsMainThread()) {
      GetMemoryCache()->Remove(resource);
      return false;
    }

    ResourceResponse response;

    blink::probe::PlatformSendRequest probe(&Context(), resource->Identifier(),
                                            request, response,
                                             resource->Options().initiator_info);
 
     Context().DispatchWillSendRequest(resource->Identifier(), request, response,
                                      resource->GetType(),
                                       resource->Options().initiator_info);
 
    SecurityOrigin* source_origin = Context().GetSecurityOrigin();
    if (source_origin && source_origin->HasSuborigin())
      request.SetServiceWorkerMode(WebURLRequest::ServiceWorkerMode::kNone);

    resource->SetResourceRequest(request);

    loader = ResourceLoader::Create(this, scheduler_, resource);
    if (resource->ShouldBlockLoadEvent())
      loaders_.insert(loader);
    else
      non_blocking_loaders_.insert(loader);

    StorePerformanceTimingInitiatorInformation(resource);
    resource->SetFetcherSecurityOrigin(source_origin);

    Resource::ProhibitAddRemoveClientInScope
        prohibit_add_remove_client_in_scope(resource);

    resource->NotifyStartLoad();
  }

  loader->Start();
  return true;
}
