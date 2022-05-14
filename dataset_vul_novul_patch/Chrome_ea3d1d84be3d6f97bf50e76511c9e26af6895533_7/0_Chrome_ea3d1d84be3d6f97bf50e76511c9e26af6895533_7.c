void WebPluginProxy::InitiateHTTPRangeRequest(const char* url,
void WebPluginProxy::InitiateHTTPRangeRequest(
    const char* url, const char* range_info, int range_request_id) {
  Send(new PluginHostMsg_InitiateHTTPRangeRequest(
      route_id_, url, range_info, range_request_id));
 }
