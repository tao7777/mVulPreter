 Response PageHandler::SetDownloadBehavior(const std::string& behavior,
                                           Maybe<std::string> download_path) {
  if (!allow_set_download_behavior_)
    return Response::Error("Not allowed.");

   WebContentsImpl* web_contents = GetWebContents();
   if (!web_contents)
     return Response::InternalError();

  if (behavior == Page::SetDownloadBehavior::BehaviorEnum::Allow &&
      !download_path.isJust())
    return Response::Error("downloadPath not provided");

  if (behavior == Page::SetDownloadBehavior::BehaviorEnum::Default) {
    DevToolsDownloadManagerHelper::RemoveFromWebContents(web_contents);
    download_manager_delegate_ = nullptr;
    return Response::OK();
  }

  content::BrowserContext* browser_context = web_contents->GetBrowserContext();
  DCHECK(browser_context);
  content::DownloadManager* download_manager =
      content::BrowserContext::GetDownloadManager(browser_context);
  download_manager_delegate_ =
      DevToolsDownloadManagerDelegate::TakeOver(download_manager);

  DevToolsDownloadManagerHelper::CreateForWebContents(web_contents);
  DevToolsDownloadManagerHelper* download_helper =
      DevToolsDownloadManagerHelper::FromWebContents(web_contents);

  download_helper->SetDownloadBehavior(
      DevToolsDownloadManagerHelper::DownloadBehavior::DENY);
  if (behavior == Page::SetDownloadBehavior::BehaviorEnum::Allow) {
    download_helper->SetDownloadBehavior(
        DevToolsDownloadManagerHelper::DownloadBehavior::ALLOW);
    download_helper->SetDownloadPath(download_path.fromJust());
  }

  return Response::OK();
}
