 void FrameImpl::GoBack() {
  if (web_contents_->GetController().CanGoBack())
    web_contents_->GetController().GoBack();
 }
