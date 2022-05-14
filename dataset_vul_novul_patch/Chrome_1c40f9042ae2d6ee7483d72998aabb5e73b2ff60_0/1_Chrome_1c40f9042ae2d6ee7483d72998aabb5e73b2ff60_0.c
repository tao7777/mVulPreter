void InspectorNetworkAgent::DidReceiveResourceResponse(
    unsigned long identifier,
    DocumentLoader* loader,
    const ResourceResponse& response,
    Resource* cached_resource) {
  String request_id = IdentifiersFactory::RequestId(identifier);
  bool is_not_modified = response.HttpStatusCode() == 304;

  bool resource_is_empty = true;
  std::unique_ptr<protocol::Network::Response> resource_response =
      BuildObjectForResourceResponse(response, cached_resource,
                                      &resource_is_empty);
 
   InspectorPageAgent::ResourceType type =
      cached_resource ? InspectorPageAgent::CachedResourceType(*cached_resource)
                      : InspectorPageAgent::kOtherResource;
   InspectorPageAgent::ResourceType saved_type =
       resources_data_->GetResourceType(request_id);
  if (saved_type == InspectorPageAgent::kScriptResource ||
      saved_type == InspectorPageAgent::kXHRResource ||
      saved_type == InspectorPageAgent::kDocumentResource ||
      saved_type == InspectorPageAgent::kFetchResource ||
      saved_type == InspectorPageAgent::kEventSourceResource) {
    type = saved_type;
  }
  if (type == InspectorPageAgent::kDocumentResource && loader &&
      loader->GetSubstituteData().IsValid())
    return;

  if (cached_resource)
    resources_data_->AddResource(request_id, cached_resource);
  String frame_id = loader && loader->GetFrame()
                        ? IdentifiersFactory::FrameId(loader->GetFrame())
                        : "";
  String loader_id = loader ? IdentifiersFactory::LoaderId(loader) : "";
  resources_data_->ResponseReceived(request_id, frame_id, response);
  resources_data_->SetResourceType(request_id, type);

  if (response.GetSecurityStyle() != ResourceResponse::kSecurityStyleUnknown &&
      response.GetSecurityStyle() !=
          ResourceResponse::kSecurityStyleUnauthenticated) {
    const ResourceResponse::SecurityDetails* response_security_details =
        response.GetSecurityDetails();
    resources_data_->SetCertificate(request_id,
                                    response_security_details->certificate);
  }

  if (resource_response && !resource_is_empty) {
    Maybe<String> maybe_frame_id;
    if (!frame_id.IsEmpty())
      maybe_frame_id = frame_id;
    GetFrontend()->responseReceived(
        request_id, loader_id, MonotonicallyIncreasingTime(),
        InspectorPageAgent::ResourceTypeJson(type),
        std::move(resource_response), std::move(maybe_frame_id));
  }
  if (is_not_modified && cached_resource && cached_resource->EncodedSize())
    DidReceiveData(identifier, loader, 0, cached_resource->EncodedSize());
}
