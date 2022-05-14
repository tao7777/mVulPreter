void InspectorNetworkAgent::WillSendRequest(
    ExecutionContext* execution_context,
    unsigned long identifier,
     DocumentLoader* loader,
     ResourceRequest& request,
     const ResourceResponse& redirect_response,
    const FetchInitiatorInfo& initiator_info,
    Resource::Type resource_type) {
   if (initiator_info.name == FetchInitiatorTypeNames::internal)
     return;

  if (initiator_info.name == FetchInitiatorTypeNames::document &&
      loader->GetSubstituteData().IsValid())
    return;

  protocol::DictionaryValue* headers =
      state_->getObject(NetworkAgentState::kExtraRequestHeaders);
  if (headers) {
    for (size_t i = 0; i < headers->size(); ++i) {
      auto header = headers->at(i);
      String value;
      if (header.second->asString(&value))
        request.SetHTTPHeaderField(AtomicString(header.first),
                                   AtomicString(value));
    }
  }

  request.SetReportRawHeaders(true);

  if (state_->booleanProperty(NetworkAgentState::kCacheDisabled, false)) {
    if (LoadsFromCacheOnly(request) &&
        request.GetRequestContext() != WebURLRequest::kRequestContextInternal) {
      request.SetCachePolicy(WebCachePolicy::kBypassCacheLoadOnlyFromCache);
    } else {
      request.SetCachePolicy(WebCachePolicy::kBypassingCache);
    }
    request.SetShouldResetAppCache(true);
  }
   if (state_->booleanProperty(NetworkAgentState::kBypassServiceWorker, false))
     request.SetServiceWorkerMode(WebURLRequest::ServiceWorkerMode::kNone);
 
  InspectorPageAgent::ResourceType type =
      InspectorPageAgent::ToResourceType(resource_type);

   WillSendRequestInternal(execution_context, identifier, loader, request,
                          redirect_response, initiator_info, type);
 
   if (!host_id_.IsEmpty()) {
     request.AddHTTPHeaderField(
        HTTPNames::X_DevTools_Emulate_Network_Conditions_Client_Id,
        AtomicString(host_id_));
  }
}
