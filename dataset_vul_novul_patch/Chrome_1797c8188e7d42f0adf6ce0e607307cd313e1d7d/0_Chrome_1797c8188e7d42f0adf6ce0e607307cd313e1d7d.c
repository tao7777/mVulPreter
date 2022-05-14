void DownloadManager::startDownload(uint64_t downloadID, const ResourceRequest& request)
void DownloadManager::startDownload(uint64_t downloadID, WebPage* initiatingPage, const ResourceRequest& request)
 {
     OwnPtr<Download> download = Download::create(downloadID, request);
    download->start(initiatingPage);
 
     ASSERT(!m_downloads.contains(downloadID));
     m_downloads.set(downloadID, download.leakPtr());
}
