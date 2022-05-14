 void WebPluginDelegatePepper::DidFinishLoadWithReason(
    const GURL& url, NPReason reason, int notify_id) {
  instance()->DidFinishLoadWithReason(url, reason, notify_id);
 }
