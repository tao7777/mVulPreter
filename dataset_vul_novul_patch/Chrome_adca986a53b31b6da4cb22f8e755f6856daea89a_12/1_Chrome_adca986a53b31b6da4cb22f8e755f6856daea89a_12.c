void WebContentsImpl::LoadingStateChanged(bool to_different_document,
                                          bool due_to_interstitial,
                                           LoadNotificationDetails* details) {
  if (ShowingInterstitialPage() &&
      GetRenderManager()->interstitial_page()->pause_throbber() &&
       !due_to_interstitial) {
     return;
   }

  bool is_loading = IsLoading();

  if (!is_loading) {
    load_state_ = net::LoadStateWithParam(net::LOAD_STATE_IDLE,
                                          base::string16());
    load_state_host_.clear();
    upload_size_ = 0;
    upload_position_ = 0;
  }

  GetRenderManager()->SetIsLoading(is_loading);

  waiting_for_response_ = is_loading;
  is_load_to_different_document_ = to_different_document;

  if (delegate_)
    delegate_->LoadingStateChanged(this, to_different_document);
  NotifyNavigationStateChanged(INVALIDATE_TYPE_LOAD);

  std::string url = (details ? details->url.possibly_invalid_spec() : "NULL");
  if (is_loading) {
    TRACE_EVENT_ASYNC_BEGIN2("browser,navigation", "WebContentsImpl Loading",
                             this, "URL", url, "Main FrameTreeNode id",
                             GetFrameTree()->root()->frame_tree_node_id());
    for (auto& observer : observers_)
      observer.DidStartLoading();
  } else {
    TRACE_EVENT_ASYNC_END1("browser,navigation", "WebContentsImpl Loading",
                           this, "URL", url);
    for (auto& observer : observers_)
      observer.DidStopLoading();
  }

  int type = is_loading ? NOTIFICATION_LOAD_START : NOTIFICATION_LOAD_STOP;
  NotificationDetails det = NotificationService::NoDetails();
  if (details)
      det = Details<LoadNotificationDetails>(details);
  NotificationService::current()->Notify(
      type, Source<NavigationController>(&controller_), det);
}
