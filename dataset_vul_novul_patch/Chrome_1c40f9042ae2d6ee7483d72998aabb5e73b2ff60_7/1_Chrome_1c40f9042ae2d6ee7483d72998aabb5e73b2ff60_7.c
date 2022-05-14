 void FrameFetchContext::DispatchDidBlockRequest(
     const ResourceRequest& resource_request,
     const FetchInitiatorInfo& fetch_initiator_info,
    ResourceRequestBlockedReason blocked_reason) const {
   if (IsDetached())
     return;
   probe::didBlockRequest(GetFrame()->GetDocument(), resource_request,
                          MasterDocumentLoader(), fetch_initiator_info,
                         blocked_reason);
 }
