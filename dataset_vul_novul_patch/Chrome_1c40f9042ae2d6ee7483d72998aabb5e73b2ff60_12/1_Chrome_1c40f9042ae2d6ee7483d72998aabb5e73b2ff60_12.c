void ResourceFetcher::DidLoadResourceFromMemoryCache(
    unsigned long identifier,
    Resource* resource,
    const ResourceRequest& original_resource_request) {
  ResourceRequest resource_request(resource->Url());
  resource_request.SetFrameType(original_resource_request.GetFrameType());
  resource_request.SetRequestContext(
       original_resource_request.GetRequestContext());
   Context().DispatchDidLoadResourceFromMemoryCache(identifier, resource_request,
                                                    resource->GetResponse());
  Context().DispatchWillSendRequest(identifier, resource_request,
                                    ResourceResponse() /* redirects */,
                                    resource->Options().initiator_info);
   Context().DispatchDidReceiveResponse(
       identifier, resource->GetResponse(), resource_request.GetFrameType(),
       resource_request.GetRequestContext(), resource,
      FetchContext::ResourceResponseType::kFromMemoryCache);

  if (resource->EncodedSize() > 0)
    Context().DispatchDidReceiveData(identifier, 0, resource->EncodedSize());

  Context().DispatchDidFinishLoading(
      identifier, 0, 0, resource->GetResponse().DecodedBodyLength());
}
