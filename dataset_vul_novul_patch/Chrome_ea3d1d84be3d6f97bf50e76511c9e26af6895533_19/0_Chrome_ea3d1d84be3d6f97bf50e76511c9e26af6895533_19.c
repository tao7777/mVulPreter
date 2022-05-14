 void WebPluginDelegateProxy::OnInitiateHTTPRangeRequest(
    const std::string& url,
    const std::string& range_info,
    int range_request_id) {
  plugin_->InitiateHTTPRangeRequest(
      url.c_str(), range_info.c_str(), range_request_id);
 }
