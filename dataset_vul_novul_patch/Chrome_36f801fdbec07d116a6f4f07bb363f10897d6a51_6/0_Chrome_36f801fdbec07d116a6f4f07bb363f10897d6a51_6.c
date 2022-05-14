void ChromeClientImpl::Focus() {
void ChromeClientImpl::Focus(LocalFrame* calling_frame) {
  if (web_view_->Client()) {
    web_view_->Client()->DidFocus(
        calling_frame ? WebLocalFrameImpl::FromFrame(calling_frame) : nullptr);
  }
 }
