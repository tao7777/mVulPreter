 DownloadCoreServiceImpl::GetDownloadManagerDelegate() {
   DownloadManager* manager = BrowserContext::GetDownloadManager(profile_);
  if (download_manager_created_) {
    DCHECK(static_cast<DownloadManagerDelegate*>(manager_delegate_.get()) ==
           manager->GetDelegate());
     return manager_delegate_.get();
  }
   download_manager_created_ = true;
 
  if (!manager_delegate_.get())
    manager_delegate_.reset(new ChromeDownloadManagerDelegate(profile_));

  manager_delegate_->SetDownloadManager(manager);

#if BUILDFLAG(ENABLE_EXTENSIONS)
  extension_event_router_.reset(
      new extensions::ExtensionDownloadsEventRouter(profile_, manager));
#endif

  if (!profile_->IsOffTheRecord()) {
    history::HistoryService* history = HistoryServiceFactory::GetForProfile(
        profile_, ServiceAccessType::EXPLICIT_ACCESS);
    history->GetNextDownloadId(
        manager_delegate_->GetDownloadIdReceiverCallback());
    download_history_.reset(new DownloadHistory(
        manager, std::unique_ptr<DownloadHistory::HistoryAdapter>(
                     new DownloadHistory::HistoryAdapter(history))));
  }

  download_ui_.reset(new DownloadUIController(
      manager, std::unique_ptr<DownloadUIController::Delegate>()));
 
   g_browser_process->download_status_updater()->AddManager(manager);
 
   return manager_delegate_.get();
}
