void WebPluginProxy::HandleURLRequest(const char *method,
void WebPluginProxy::HandleURLRequest(const char* url,
                                      const char *method,
                                      const char* target,
                                      const char* buf,
                                      unsigned int len,
                                      int notify_id,
                                       bool popups_allowed) {
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
  params.url = url;
   params.method = method;
   if (target)
     params.target = std::string(target);
 
  if (len) {
    params.buffer.resize(len);
     memcpy(&params.buffer.front(), buf, len);
   }
 
  params.notify_id = notify_id;
   params.popups_allowed = popups_allowed;
 
   Send(new PluginHostMsg_URLRequest(route_id_, params));
}
