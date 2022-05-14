 void WebPluginDelegateStub::OnHandleURLRequestReply(
    unsigned long resource_id, const GURL& url, int notify_id) {
   WebPluginResourceClient* resource_client =
      delegate_->CreateResourceClient(resource_id, url, notify_id);
  webplugin_->OnResourceCreated(resource_id, resource_client);
}

void WebPluginDelegateStub::OnHTTPRangeRequestReply(
    unsigned long resource_id, int range_request_id) {
  WebPluginResourceClient* resource_client =
      delegate_->CreateSeekableResourceClient(resource_id, range_request_id);
  webplugin_->OnResourceCreated(resource_id, resource_client);
 }
