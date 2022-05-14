void WebPluginImpl::HandleURLRequest(const char *method,
                                     bool is_javascript_url,
                                     const char* target, unsigned int len,
                                     const char* buf, bool is_file_data,
                                     bool notify, const char* url,
                                     intptr_t notify_data, bool popups_allowed) {
  HandleURLRequestInternal(method, is_javascript_url, target, len, buf,
                           is_file_data, notify, url, notify_data,
                           popups_allowed, PLUGIN_SRC);
 }
