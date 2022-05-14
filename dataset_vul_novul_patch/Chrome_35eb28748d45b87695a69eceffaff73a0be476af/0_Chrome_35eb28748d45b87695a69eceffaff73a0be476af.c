bool BackgroundLoaderOffliner::LoadAndSave(
    const SavePageRequest& request,
    CompletionCallback completion_callback,
    const ProgressCallback& progress_callback) {
  DCHECK(completion_callback);
  DCHECK(progress_callback);
  DCHECK(offline_page_model_);

  if (pending_request_) {
    DVLOG(1) << "Already have pending request";
    return false;
  }

  ClientPolicyController* policy_controller =
      offline_page_model_->GetPolicyController();
  if (policy_controller->RequiresSpecificUserSettings(
          request.client_id().name_space) &&
      (AreThirdPartyCookiesBlocked(browser_context_) ||
        IsNetworkPredictionDisabled(browser_context_))) {
     DVLOG(1) << "WARNING: Unable to load when 3rd party cookies blocked or "
              << "prediction disabled";
     return false;
   }
 
   if (!OfflinePageModel::CanSaveURL(request.url())) {
     DVLOG(1) << "Not able to save page for requested url: " << request.url();
     return false;
  }

  ResetLoader();
  AttachObservers();

  MarkLoadStartTime();

  pending_request_.reset(new SavePageRequest(request));
  completion_callback_ = std::move(completion_callback);
  progress_callback_ = progress_callback;

  if (IsOfflinePagesRenovationsEnabled()) {
    if (!page_renovation_loader_)
      page_renovation_loader_ = std::make_unique<PageRenovationLoader>();

    auto script_injector = std::make_unique<RenderFrameScriptInjector>(
        loader_->web_contents()->GetMainFrame(),
        ISOLATED_WORLD_ID_CHROME_INTERNAL);
    page_renovator_ = std::make_unique<PageRenovator>(
        page_renovation_loader_.get(), std::move(script_injector),
        request.url());
  }

  loader_.get()->LoadPage(request.url());

  snapshot_controller_ = std::make_unique<BackgroundSnapshotController>(
      base::ThreadTaskRunnerHandle::Get(), this,
      static_cast<bool>(page_renovator_));

  return true;
}
