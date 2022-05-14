 void WebPluginDelegateProxy::SendJavaScriptStream(const GURL& url,
                                                   const std::string& result,
                                                   bool success,
                                                  bool notify_needed,
                                                  intptr_t notify_data) {
  PluginMsg_SendJavaScriptStream* msg =
      new PluginMsg_SendJavaScriptStream(instance_id_, url, result,
                                         success, notify_needed,
                                         notify_data);
  Send(msg);
 }
