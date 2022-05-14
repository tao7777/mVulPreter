void DownloadRequestLimiter::TabDownloadState::DidStartNavigation(
    content::NavigationHandle* navigation_handle) {
  if (!navigation_handle->IsInMainFrame())
    return;

   download_seen_ = false;
   ui_status_ = DOWNLOAD_UI_DEFAULT;
 
  if (status_ == PROMPT_BEFORE_DOWNLOAD || status_ == DOWNLOADS_NOT_ALLOWED) {
    std::string host = navigation_handle->GetURL().host();
    // If the navigation is renderer-initiated (but not user-initiated), ensure
    // that a prompting or blocking limiter state is not reset, so
    // window.location.href or meta refresh can't be abused to avoid the
    // limiter.
    if (navigation_handle->IsRendererInitiated()) {
      if (!host.empty())
        restricted_hosts_.emplace(host);
      return;
    }

    // If this is a forward/back navigation, also don't reset a prompting or
    // blocking limiter state unless a new host is encounted. This prevents a
    // page to use history forward/backward to trigger multiple downloads.
    if (IsNavigationRestricted(navigation_handle))
      return;
   }
 
   if (status_ == DownloadRequestLimiter::ALLOW_ALL_DOWNLOADS ||
      status_ == DownloadRequestLimiter::DOWNLOADS_NOT_ALLOWED) {
    if (!initial_page_host_.empty() &&
        navigation_handle->GetURL().host_piece() == initial_page_host_) {
      return;
    }
  }

  NotifyCallbacks(false);
  host_->Remove(this, web_contents());
}
