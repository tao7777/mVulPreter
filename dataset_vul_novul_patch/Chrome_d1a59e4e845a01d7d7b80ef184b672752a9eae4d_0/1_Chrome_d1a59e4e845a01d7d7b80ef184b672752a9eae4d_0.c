void RenderViewImpl::OnPostMessageEvent(
    const ViewMsg_PostMessage_Params& params) {
  WebFrame* frame = FindFrameByID(webview()->mainFrame(),
                                  params.target_frame_id);
  if (!frame)
    return;

  WebFrame* source_frame = NULL;
   if (params.source_routing_id != MSG_ROUTING_NONE) {
     RenderViewImpl* source_view = FromRoutingID(params.source_routing_id);
     if (source_view)
      source_frame = source_view->GetFrameByMappedID(params.source_frame_id);
   }
 
  WebDOMEvent event = frame->document().createEvent("MessageEvent");
  WebDOMMessageEvent msg_event = event.to<WebDOMMessageEvent>();
  msg_event.initMessageEvent("message",
                             false, false,
                             WebSerializedScriptValue::fromString(params.data),
                             params.source_origin, source_frame, "");

  WebSecurityOrigin target_origin;
  if (!params.target_origin.empty()) {
    target_origin =
        WebSecurityOrigin::createFromString(WebString(params.target_origin));
  }
  frame->dispatchMessageEventWithOriginCheck(target_origin, msg_event);
}
