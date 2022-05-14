void WebPluginImpl::InitiateHTTPRangeRequest(const char* url,
                                             const char* range_info,
                                             intptr_t existing_stream,
                                             bool notify_needed,
                                             intptr_t notify_data) {
   unsigned long resource_id = GetNextResourceId();
   if (!resource_id)
     return;
 
   GURL complete_url = CompleteURL(url);
 
  WebPluginResourceClient* resource_client = delegate_->CreateResourceClient(
      resource_id, complete_url, notify_needed, notify_data, existing_stream);
   InitiateHTTPRequest(
      resource_id, resource_client, "GET", NULL, 0, complete_url, range_info,
       load_manually_ ? NO_REFERRER : PLUGIN_SRC);
 }
