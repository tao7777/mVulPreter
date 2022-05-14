void FrameFetchContext::DispatchWillSendRequest(
     unsigned long identifier,
     ResourceRequest& request,
     const ResourceResponse& redirect_response,
    Resource::Type resource_type,
     const FetchInitiatorInfo& initiator_info) {
   if (IsDetached())
     return;

  if (redirect_response.IsNull()) {
    GetFrame()->Loader().Progress().WillStartLoading(identifier,
                                                     request.Priority());
   }
   probe::willSendRequest(GetFrame()->GetDocument(), identifier,
                          MasterDocumentLoader(), request, redirect_response,
                         initiator_info, resource_type);
   if (IdlenessDetector* idleness_detector = GetFrame()->GetIdlenessDetector())
     idleness_detector->OnWillSendRequest();
   if (GetFrame()->FrameScheduler())
    GetFrame()->FrameScheduler()->DidStartLoading(identifier);
}
