void WebPluginProxy::InitiateHTTPRangeRequest(const char* url,
                                              const char* range_info,
                                              intptr_t existing_stream,
                                              bool notify_needed,
                                              intptr_t notify_data) {
  Send(new PluginHostMsg_InitiateHTTPRangeRequest(route_id_, url,
                                                  range_info, existing_stream,
                                                  notify_needed, notify_data));
 }
