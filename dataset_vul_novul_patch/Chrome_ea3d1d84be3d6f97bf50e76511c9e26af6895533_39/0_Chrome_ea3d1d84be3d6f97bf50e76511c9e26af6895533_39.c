void WebPluginImpl::InitiateHTTPRangeRequest(const char* url,
void WebPluginImpl::InitiateHTTPRangeRequest(
    const char* url, const char* range_info, int range_request_id) {
   unsigned long resource_id = GetNextResourceId();
   if (!resource_id)
     return;
 
   GURL complete_url = CompleteURL(url);
 
  WebPluginResourceClient* resource_client =
      delegate_->CreateSeekableResourceClient(resource_id, range_request_id);
   InitiateHTTPRequest(
      resource_id, resource_client, complete_url, "GET", NULL, 0, range_info,
       load_manually_ ? NO_REFERRER : PLUGIN_SRC);
 }
