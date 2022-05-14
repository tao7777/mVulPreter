 void WebPluginDelegateStub::OnSendJavaScriptStream(const GURL& url,
                                                    const std::string& result,
                                                    bool success,
                                                   bool notify_needed,
                                                   intptr_t notify_data) {
  delegate_->SendJavaScriptStream(url, result, success, notify_needed,
                                  notify_data);
 }
