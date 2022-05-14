void DownloadRequestLimiter::TabDownloadState::SetDownloadStatusAndNotifyImpl(
    DownloadStatus status,
    ContentSetting setting) {
  DCHECK((GetSettingFromDownloadStatus(status) == setting) ||
         (GetDownloadStatusFromSetting(setting) == status))
      << "status " << status << " and setting " << setting
      << " do not correspond to each other";

  ContentSetting last_setting = GetSettingFromDownloadStatus(status_);
  DownloadUiStatus last_ui_status = ui_status_;

  status_ = status;
  ui_status_ = GetUiStatusFromDownloadStatus(status_, download_seen_);

   if (!web_contents())
     return;
 
  if (status_ == PROMPT_BEFORE_DOWNLOAD || status_ == DOWNLOADS_NOT_ALLOWED) {
    if (!initial_page_host_.empty())
      restricted_hosts_.emplace(initial_page_host_);
  }

  if (last_setting == setting && last_ui_status == ui_status_)
    return;

  content::NotificationService::current()->Notify(
      chrome::NOTIFICATION_WEB_CONTENT_SETTINGS_CHANGED,
      content::Source<content::WebContents>(web_contents()),
       content::NotificationService::NoDetails());
 }
