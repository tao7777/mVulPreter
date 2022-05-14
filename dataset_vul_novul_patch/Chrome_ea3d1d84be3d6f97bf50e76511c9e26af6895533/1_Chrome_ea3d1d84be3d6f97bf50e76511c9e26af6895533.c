 void WebPluginDelegateStub::OnDidFinishLoadWithReason(
    const GURL& url, int reason, intptr_t notify_data) {
  delegate_->DidFinishLoadWithReason(url, reason, notify_data);
 }
