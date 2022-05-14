void WebPluginImpl::HandleURLRequestInternal(
void WebPluginImpl::HandleURLRequestInternal(const char* url,
                                             const char *method,
                                             const char* target,
                                             const char* buf,
                                             unsigned int len,
                                             int notify_id,
                                             bool popups_allowed,
                                             Referrer referrer_flag) {
  bool is_javascript_url = StartsWithASCII(url, "javascript:", false);
  RoutingStatus routing_status = RouteToFrame(
      url, is_javascript_url, method, target, buf, len, notify_id,
      referrer_flag);
   if (routing_status == ROUTED)
     return;
 
  if (is_javascript_url) {
    GURL gurl(url);
    WebString result = container_->executeScriptURL(gurl, popups_allowed);

     if (delegate_) {
       delegate_->SendJavaScriptStream(
          gurl, result.utf8(), !result.isNull(), notify_id);
     }
 
    return;
  }
 
  unsigned long resource_id = GetNextResourceId();
  if (!resource_id)
    return;
 
  GURL complete_url = CompleteURL(url);
  WebPluginResourceClient* resource_client = delegate_->CreateResourceClient(
      resource_id, complete_url, notify_id);
  if (!resource_client)
    return;
 
  // If the RouteToFrame call returned a failure then inform the result
  // back to the plugin asynchronously.
  if ((routing_status == INVALID_URL) ||
      (routing_status == GENERAL_FAILURE)) {
    resource_client->DidFail();
    return;
   }

  // CreateResourceClient() sends a synchronous IPC message so it's possible
  // that TearDownPluginInstance() may have been called in the nested
  // message loop.  If so, don't start the request.
  if (!delegate_)
    return;

  InitiateHTTPRequest(resource_id, resource_client, complete_url, method, buf,
                      len, NULL, referrer_flag);
 }
