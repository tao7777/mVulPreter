 bool WebPluginImpl::InitiateHTTPRequest(unsigned long resource_id,
                                         WebPluginResourceClient* client,
                                        const char* method, const char* buf,
                                        int buf_len,
                                         const GURL& url,
                                         const char* range_info,
                                         Referrer referrer_flag) {
   if (!client) {
    NOTREACHED();
    return false;
  }

  ClientInfo info;
  info.id = resource_id;
  info.client = client;
  info.request.initialize();
  info.request.setURL(url);
  info.request.setRequestorProcessID(delegate_->GetProcessId());
  info.request.setTargetType(WebURLRequest::TargetIsObject);
  info.request.setHTTPMethod(WebString::fromUTF8(method));
  info.pending_failure_notification = false;

  if (range_info) {
    info.request.addHTTPHeaderField(WebString::fromUTF8("Range"),
                                    WebString::fromUTF8(range_info));
  }

  if (strcmp(method, "POST") == 0) {
    SetPostData(&info.request, buf, buf_len);
  }

  SetReferrer(&info.request, referrer_flag);

  webframe_->dispatchWillSendRequest(info.request);
  if (WebDevToolsAgent* devtools_agent = GetDevToolsAgent()) {
    devtools_agent->identifierForInitialRequest(resource_id, webframe_,
                                                info.request);
    devtools_agent->willSendRequest(resource_id, info.request);
  }

  info.loader.reset(WebKit::webKitClient()->createURLLoader());
  if (!info.loader.get())
    return false;
  info.loader->loadAsynchronously(info.request, this);

  clients_.push_back(info);
  return true;
}
