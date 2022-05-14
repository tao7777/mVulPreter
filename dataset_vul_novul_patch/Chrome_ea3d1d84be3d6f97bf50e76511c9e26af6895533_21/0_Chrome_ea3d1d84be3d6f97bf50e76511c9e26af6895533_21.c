 void WebPluginDelegateProxy::SendJavaScriptStream(const GURL& url,
                                                   const std::string& result,
                                                   bool success,
                                                  int notify_id) {
  Send(new PluginMsg_SendJavaScriptStream(
      instance_id_, url, result, success, notify_id));
 }
