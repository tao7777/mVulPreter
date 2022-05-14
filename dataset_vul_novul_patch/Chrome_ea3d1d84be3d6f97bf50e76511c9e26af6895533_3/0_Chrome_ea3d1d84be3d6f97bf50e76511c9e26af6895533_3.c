 void WebPluginDelegateStub::OnSendJavaScriptStream(const GURL& url,
                                                    const std::string& result,
                                                    bool success,
                                                   int notify_id) {
  delegate_->SendJavaScriptStream(url, result, success, notify_id);
 }
