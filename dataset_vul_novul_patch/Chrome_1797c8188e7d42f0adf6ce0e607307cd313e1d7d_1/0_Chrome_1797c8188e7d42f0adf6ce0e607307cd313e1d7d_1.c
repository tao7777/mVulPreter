void WebFrame::startDownload(const WebCore::ResourceRequest& request)
 {
     ASSERT(m_policyDownloadID);
 
    DownloadManager::shared().startDownload(m_policyDownloadID, page(), request);
 
     m_policyDownloadID = 0;
 }
