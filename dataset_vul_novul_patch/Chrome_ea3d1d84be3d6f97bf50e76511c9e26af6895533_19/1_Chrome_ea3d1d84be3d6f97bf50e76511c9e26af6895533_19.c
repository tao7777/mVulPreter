 void WebPluginDelegateProxy::OnInitiateHTTPRangeRequest(
    const std::string& url, const std::string& range_info,
    intptr_t existing_stream, bool notify_needed, intptr_t notify_data) {
  plugin_->InitiateHTTPRangeRequest(url.c_str(), range_info.c_str(),
                                    existing_stream, notify_needed,
                                    notify_data);
 }
