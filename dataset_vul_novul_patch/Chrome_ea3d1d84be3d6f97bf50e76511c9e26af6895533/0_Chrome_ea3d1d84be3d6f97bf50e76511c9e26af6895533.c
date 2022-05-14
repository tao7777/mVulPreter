 void WebPluginDelegateStub::OnDidFinishLoadWithReason(
    const GURL& url, int reason, int notify_id) {
  delegate_->DidFinishLoadWithReason(url, reason, notify_id);
 }
