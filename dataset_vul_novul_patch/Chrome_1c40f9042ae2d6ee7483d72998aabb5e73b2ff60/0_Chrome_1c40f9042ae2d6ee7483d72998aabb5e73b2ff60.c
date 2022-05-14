void InspectorNetworkAgent::DidBlockRequest(
    ExecutionContext* execution_context,
     const ResourceRequest& request,
     DocumentLoader* loader,
     const FetchInitiatorInfo& initiator_info,
    ResourceRequestBlockedReason reason,
    Resource::Type resource_type) {
   unsigned long identifier = CreateUniqueIdentifier();
  InspectorPageAgent::ResourceType type =
      InspectorPageAgent::ToResourceType(resource_type);

   WillSendRequestInternal(execution_context, identifier, loader, request,
                          ResourceResponse(), initiator_info, type);
 
   String request_id = IdentifiersFactory::RequestId(identifier);
   String protocol_reason = BuildBlockedReason(reason);
  GetFrontend()->loadingFailed(
      request_id, MonotonicallyIncreasingTime(),
      InspectorPageAgent::ResourceTypeJson(
          resources_data_->GetResourceType(request_id)),
      String(), false, protocol_reason);
}
