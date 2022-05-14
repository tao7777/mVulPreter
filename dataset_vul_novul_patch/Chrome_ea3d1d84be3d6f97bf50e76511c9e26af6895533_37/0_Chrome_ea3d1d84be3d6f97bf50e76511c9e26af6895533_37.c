void WebPluginImpl::HandleURLRequest(const char *method,
void WebPluginImpl::HandleURLRequest(const char* url,
                                     const char *method,
                                     const char* target,
                                     const char* buf,
                                     unsigned int len,
                                     int notify_id,
                                     bool popups_allowed) {
  HandleURLRequestInternal(
      url, method, target, buf, len, notify_id, popups_allowed, PLUGIN_SRC);
 }
