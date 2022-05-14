void DownloadManagerImpl::CreateNewDownloadItemToStart(
    std::unique_ptr<download::DownloadCreateInfo> info,
    const download::DownloadUrlParameters::OnStartedCallback& on_started,
    download::InProgressDownloadManager::StartDownloadItemCallback callback,
    uint32_t id) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);

   download::DownloadItemImpl* download = CreateActiveItem(id, *info);
   std::move(callback).Run(std::move(info), download,
                           should_persist_new_download_);
  if (download) {
    // For new downloads, we notify here, rather than earlier, so that
    // the download_file is bound to download and all the usual
    // setters (e.g. Cancel) work.
    for (auto& observer : observers_)
      observer.OnDownloadCreated(this, download);
    OnNewDownloadCreated(download);
  }
 
   OnDownloadStarted(download, on_started);
 }
