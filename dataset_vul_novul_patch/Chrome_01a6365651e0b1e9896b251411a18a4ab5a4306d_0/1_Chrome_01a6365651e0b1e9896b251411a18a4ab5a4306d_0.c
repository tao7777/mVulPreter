void DownloadRequestLimiter::TabDownloadState::DidStartNavigation(
    content::NavigationHandle* navigation_handle) {
  if (!navigation_handle->IsInMainFrame())
    return;

   download_seen_ = false;
   ui_status_ = DOWNLOAD_UI_DEFAULT;
 
  if (navigation_handle->IsRendererInitiated() &&
      (status_ == PROMPT_BEFORE_DOWNLOAD || status_ == DOWNLOADS_NOT_ALLOWED)) {
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
