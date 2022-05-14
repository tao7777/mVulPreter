void InspectorTraceEvents::WillSendRequest(
    ExecutionContext*,
    unsigned long identifier,
     DocumentLoader* loader,
     ResourceRequest& request,
     const ResourceResponse& redirect_response,
    const FetchInitiatorInfo&) {
   LocalFrame* frame = loader ? loader->GetFrame() : nullptr;
   TRACE_EVENT_INSTANT1(
       "devtools.timeline", "ResourceSendRequest", TRACE_EVENT_SCOPE_THREAD,
      "data", InspectorSendRequestEvent::Data(identifier, frame, request));
  probe::AsyncTaskScheduled(frame ? frame->GetDocument() : nullptr,
                            "SendRequest", AsyncId(identifier));
}
