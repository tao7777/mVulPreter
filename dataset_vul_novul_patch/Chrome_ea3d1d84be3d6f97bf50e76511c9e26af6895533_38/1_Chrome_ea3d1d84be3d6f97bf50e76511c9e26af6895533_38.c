void WebPluginImpl::HandleURLRequestInternal(
    const char *method, bool is_javascript_url, const char* target,
    unsigned int len, const char* buf, bool is_file_data, bool notify,
    const char* url, intptr_t notify_data, bool popups_allowed,
    Referrer referrer_flag) {
  RoutingStatus routing_status =
      RouteToFrame(method, is_javascript_url, target, len, buf, is_file_data,
                   notify, notify_data, url, referrer_flag);
   if (routing_status == ROUTED)
     return;
 
  if (is_javascript_url) {
    GURL gurl(url);
    WebString result = container_->executeScriptURL(gurl, popups_allowed);

     if (delegate_) {
       delegate_->SendJavaScriptStream(
          gurl, result.utf8(), !result.isNull(), notify, notify_data);
     }
  } else {
    GURL complete_url = CompleteURL(url);
 
    unsigned long resource_id = GetNextResourceId();
    if (!resource_id)
      return;
    WebPluginResourceClient* resource_client = delegate_->CreateResourceClient(
        resource_id, complete_url, notify, notify_data, NULL);
    if (!resource_client)
      return;
 
    if ((routing_status == INVALID_URL) ||
        (routing_status == GENERAL_FAILURE)) {
      resource_client->DidFail();
      return;
    }
 
    if (!delegate_)
      return;
 
    InitiateHTTPRequest(resource_id, resource_client, method, buf, len,
                        complete_url, NULL, referrer_flag);
   }
 }
