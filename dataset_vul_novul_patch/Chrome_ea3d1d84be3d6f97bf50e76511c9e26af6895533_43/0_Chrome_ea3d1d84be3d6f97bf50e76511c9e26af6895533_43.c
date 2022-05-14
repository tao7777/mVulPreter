void WebPluginImpl::didFailLoadingFrameRequest(
    const WebURL& url, void* notify_data, const WebURLError& error) {
  if (!delegate_)
    return;

   NPReason reason =
       error.reason == net::ERR_ABORTED ? NPRES_USER_BREAK : NPRES_NETWORK_ERR;
   delegate_->DidFinishLoadWithReason(
      url, reason, reinterpret_cast<int>(notify_data));
 }
