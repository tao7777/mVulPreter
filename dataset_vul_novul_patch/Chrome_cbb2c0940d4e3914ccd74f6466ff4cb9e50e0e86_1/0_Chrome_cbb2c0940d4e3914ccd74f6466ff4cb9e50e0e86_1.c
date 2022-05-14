void DownloadCoreServiceImpl::SetDownloadManagerDelegateForTesting(
    std::unique_ptr<ChromeDownloadManagerDelegate> new_delegate) {
   manager_delegate_.swap(new_delegate);
   DownloadManager* dm = BrowserContext::GetDownloadManager(profile_);
   dm->SetDelegate(manager_delegate_.get());
  if (manager_delegate_)
    manager_delegate_->SetDownloadManager(dm);
  download_manager_created_ = !!manager_delegate_;
   if (new_delegate)
     new_delegate->Shutdown();
 }
