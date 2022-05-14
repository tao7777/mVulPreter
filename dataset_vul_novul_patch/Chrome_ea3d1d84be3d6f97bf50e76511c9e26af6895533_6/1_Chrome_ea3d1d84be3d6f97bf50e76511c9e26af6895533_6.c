void WebPluginProxy::HandleURLRequest(const char *method,
                                      bool is_javascript_url,
                                      const char* target, unsigned int len,
                                      const char* buf, bool is_file_data,
                                      bool notify, const char* url,
                                      intptr_t notify_data,
                                       bool popups_allowed) {
  if (!url) {
    NOTREACHED();
    return;
  }
  if (!target && (0 == base::strcasecmp(method, "GET"))) {
     if (delegate_->GetQuirks() &
        WebPluginDelegateImpl::PLUGIN_QUIRK_BLOCK_NONSTANDARD_GETURL_REQUESTS) {
      GURL request_url(url);
      if (!request_url.SchemeIs(chrome::kHttpScheme) &&
          !request_url.SchemeIs(chrome::kHttpsScheme) &&
          !request_url.SchemeIs(chrome::kFtpScheme)) {
        return;
      }
    }
   }
 
   PluginHostMsg_URLRequest_Params params;
   params.method = method;
  params.is_javascript_url = is_javascript_url;
   if (target)
     params.target = std::string(target);
 
  if (len) {
    params.buffer.resize(len);
     memcpy(&params.buffer.front(), buf, len);
   }
 
  params.is_file_data = is_file_data;
  params.notify = notify;
  params.url = url;
  params.notify_data = notify_data;
   params.popups_allowed = popups_allowed;
 
   Send(new PluginHostMsg_URLRequest(route_id_, params));
}
