void DownloadRequestLimiter::TabDownloadState::DidFinishNavigation(
    content::NavigationHandle* navigation_handle) {
  if (!navigation_handle->IsInMainFrame())
    return;

   if (status_ == ALLOW_ONE_DOWNLOAD ||
       (status_ == PROMPT_BEFORE_DOWNLOAD &&
       !navigation_handle->IsRendererInitiated())) {
    NotifyCallbacks(false);
    host_->Remove(this, web_contents());
  }
}
