 void WebPluginDelegateImpl::SendJavaScriptStream(const GURL& url,
                                                  const std::string& result,
                                                  bool success,
                                                 int notify_id) {
  instance()->SendJavaScriptStream(url, result, success, notify_id);
 }
