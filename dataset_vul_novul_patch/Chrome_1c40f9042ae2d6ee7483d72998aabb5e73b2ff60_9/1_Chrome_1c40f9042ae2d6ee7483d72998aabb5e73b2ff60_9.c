 void WorkerFetchContext::DispatchDidBlockRequest(
     const ResourceRequest& resource_request,
     const FetchInitiatorInfo& fetch_initiator_info,
    ResourceRequestBlockedReason blocked_reason) const {
   probe::didBlockRequest(global_scope_, resource_request, nullptr,
                         fetch_initiator_info, blocked_reason);
 }
