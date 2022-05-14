 WebPluginImpl::RoutingStatus WebPluginImpl::RouteToFrame(
    const char *method,
     bool is_javascript_url,
     const char* target,
    unsigned int len,
     const char* buf,
    bool is_file_data,
    bool notify_needed,
    intptr_t notify_data,
    const char* url,
     Referrer referrer_flag) {
   if (!target)
    return NOT_ROUTED;

  if (!webframe_)
    return NOT_ROUTED;

  WebString target_str = WebString::fromUTF8(target);

  if (is_javascript_url) {
    WebFrame* target_frame =
        webframe_->view()->findFrameByName(target_str, webframe_);
    if (target_frame != webframe_) {
      const char kMessage[] =
          "Ignoring cross-frame javascript URL load requested by plugin.";
      webframe_->addMessageToConsole(
          WebConsoleMessage(WebConsoleMessage::LevelError,
                            WebString::fromUTF8(kMessage)));
      return ROUTED;
    }

    return NOT_ROUTED;
  }


  GURL complete_url = CompleteURL(url);

  if (strcmp(method, "GET") != 0) {
    if (!(complete_url.SchemeIs("http") || complete_url.SchemeIs("https")))
      return INVALID_URL;
  }

  WebURLRequest request(complete_url);
  SetReferrer(&request, referrer_flag);
 
   request.setHTTPMethod(WebString::fromUTF8(method));
   if (len > 0) {
    if (!is_file_data) {
      if (!SetPostData(&request, buf, len)) {
        NOTREACHED();
        return ROUTED;
      }
    } else {
       NOTREACHED();
       return ROUTED;
     }
   }
 
  container_->loadFrameRequest(request, target_str, notify_needed,
                               reinterpret_cast<void*>(notify_data));
   return ROUTED;
 }
