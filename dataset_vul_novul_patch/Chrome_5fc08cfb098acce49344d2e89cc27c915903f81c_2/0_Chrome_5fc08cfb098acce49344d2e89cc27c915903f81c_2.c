 void DownloadController::OnDownloadStarted(
     DownloadItem* download_item) {
   DCHECK_CURRENTLY_ON(BrowserThread::UI);
  WebContents* web_contents = download_item->GetWebContents();
  if (!web_contents)
    return;

   download_item->AddObserver(this);
 
   ChromeDownloadDelegate::FromWebContents(web_contents)->OnDownloadStarted(
      download_item->GetTargetFilePath().BaseName().value());
 }
