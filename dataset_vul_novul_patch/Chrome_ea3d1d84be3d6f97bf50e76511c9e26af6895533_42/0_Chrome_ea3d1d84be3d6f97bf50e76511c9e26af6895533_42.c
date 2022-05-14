 WebPluginImpl::RoutingStatus WebPluginImpl::RouteToFrame(
    const char* url,
     bool is_javascript_url,
    const char* method,
     const char* target,
     const char* buf,
    unsigned int len,
    int notify_id,
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
    if (!SetPostData(&request, buf, len)) {
      // Uhoh - we're in trouble.  There isn't a good way
      // to recover at this point.  Break out.
       NOTREACHED();
       return ROUTED;
     }
   }
 
  container_->loadFrameRequest(
      request, target_str, notify_id != 0, reinterpret_cast<void*>(notify_id));
   return ROUTED;
 }
