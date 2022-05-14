bool WebMediaPlayerImpl::DidPassCORSAccessCheck() const {
bool WebMediaPlayerImpl::WouldTaintOrigin() const {
  if (!HasSingleSecurityOrigin()) {
    // When the resource is redirected to another origin we think it as
    // tainted. This is actually not specified, and is under discussion.
    // See https://github.com/whatwg/fetch/issues/737.
    return true;
  }

  return data_source_ && data_source_->IsCorsCrossOrigin();
 }
