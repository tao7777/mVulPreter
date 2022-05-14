 void WebPluginDelegateProxy::DidFinishLoadWithReason(
    const GURL& url, NPReason reason, int notify_id) {
   Send(new PluginMsg_DidFinishLoadWithReason(
      instance_id_, url, reason, notify_id));
 }
