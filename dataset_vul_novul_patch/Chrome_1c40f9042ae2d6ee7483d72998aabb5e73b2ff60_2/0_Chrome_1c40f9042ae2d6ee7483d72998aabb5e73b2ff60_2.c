void InspectorNetworkAgent::WillSendRequestInternal(
    ExecutionContext* execution_context,
    unsigned long identifier,
     DocumentLoader* loader,
     const ResourceRequest& request,
     const ResourceResponse& redirect_response,
    const FetchInitiatorInfo& initiator_info,
    InspectorPageAgent::ResourceType type) {
   String request_id = IdentifiersFactory::RequestId(identifier);
   String loader_id = loader ? IdentifiersFactory::LoaderId(loader) : "";
   resources_data_->ResourceCreated(request_id, loader_id, request.Url());
  if (initiator_info.name == FetchInitiatorTypeNames::xmlhttprequest)
     type = InspectorPageAgent::kXHRResource;

  resources_data_->SetResourceType(request_id, type);
 
   String frame_id = loader && loader->GetFrame()
                         ? IdentifiersFactory::FrameId(loader->GetFrame())
                        : "";
  std::unique_ptr<protocol::Network::Initiator> initiator_object =
      BuildInitiatorObject(loader && loader->GetFrame()
                               ? loader->GetFrame()->GetDocument()
                               : nullptr,
                           initiator_info);
  if (initiator_info.name == FetchInitiatorTypeNames::document) {
    FrameNavigationInitiatorMap::iterator it =
        frame_navigation_initiator_map_.find(frame_id);
    if (it != frame_navigation_initiator_map_.end())
      initiator_object = it->value->clone();
  }

  std::unique_ptr<protocol::Network::Request> request_info(
      BuildObjectForResourceRequest(request));

  if (loader) {
    request_info->setMixedContentType(MixedContentTypeForContextType(
        MixedContentChecker::ContextTypeForInspector(loader->GetFrame(),
                                                     request)));
  }

  request_info->setReferrerPolicy(
      GetReferrerPolicy(request.GetReferrerPolicy()));
  if (initiator_info.is_link_preload)
    request_info->setIsLinkPreload(true);

  String resource_type = InspectorPageAgent::ResourceTypeJson(type);
  String documentURL =
      loader ? UrlWithoutFragment(loader->Url()).GetString()
             : UrlWithoutFragment(execution_context->Url()).GetString();
  Maybe<String> maybe_frame_id;
  if (!frame_id.IsEmpty())
    maybe_frame_id = frame_id;
  GetFrontend()->requestWillBeSent(
      request_id, loader_id, documentURL, std::move(request_info),
      MonotonicallyIncreasingTime(), CurrentTime(), std::move(initiator_object),
      BuildObjectForResourceResponse(redirect_response), resource_type,
      std::move(maybe_frame_id));
  if (pending_xhr_replay_data_ && !pending_xhr_replay_data_->Async())
    GetFrontend()->flush();
}
