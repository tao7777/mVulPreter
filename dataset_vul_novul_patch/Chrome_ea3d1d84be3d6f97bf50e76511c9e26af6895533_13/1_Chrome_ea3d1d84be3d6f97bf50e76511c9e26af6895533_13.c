 void WebPluginDelegateProxy::DidFinishLoadWithReason(
    const GURL& url, NPReason reason, intptr_t notify_data) {
   Send(new PluginMsg_DidFinishLoadWithReason(
      instance_id_, url, reason, notify_data));
 }
