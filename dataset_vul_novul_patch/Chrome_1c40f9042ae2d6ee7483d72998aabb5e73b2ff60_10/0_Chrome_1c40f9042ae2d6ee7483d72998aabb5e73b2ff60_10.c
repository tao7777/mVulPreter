void WorkerFetchContext::DispatchWillSendRequest(
     unsigned long identifier,
     ResourceRequest& request,
     const ResourceResponse& redirect_response,
    Resource::Type resource_type,
     const FetchInitiatorInfo& initiator_info) {
   probe::willSendRequest(global_scope_, identifier, nullptr, request,
                         redirect_response, initiator_info, resource_type);
 }
