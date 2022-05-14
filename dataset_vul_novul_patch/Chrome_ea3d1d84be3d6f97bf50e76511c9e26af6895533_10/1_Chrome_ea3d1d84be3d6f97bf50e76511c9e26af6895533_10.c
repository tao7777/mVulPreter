 void WebPluginDelegatePepper::DidFinishLoadWithReason(
    const GURL& url,
    NPReason reason,
    intptr_t notify_data) {
  instance()->DidFinishLoadWithReason(
      url, reason, reinterpret_cast<void*>(notify_data));
 }
