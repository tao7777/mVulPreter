 void FrameImpl::GoForward() {
  if (web_contents_->GetController().CanGoForward())
    web_contents_->GetController().GoForward();
 }
