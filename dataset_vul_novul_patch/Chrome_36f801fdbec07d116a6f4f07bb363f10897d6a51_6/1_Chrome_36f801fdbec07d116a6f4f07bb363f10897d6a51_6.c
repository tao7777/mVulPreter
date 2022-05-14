void ChromeClientImpl::Focus() {
  if (web_view_->Client())
    web_view_->Client()->DidFocus();
 }
