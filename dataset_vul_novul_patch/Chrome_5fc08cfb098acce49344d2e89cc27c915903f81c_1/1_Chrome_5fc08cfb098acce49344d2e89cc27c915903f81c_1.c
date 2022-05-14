void DownloadController::CreateGETDownload(
    const content::ResourceRequestInfo::WebContentsGetter& wc_getter,
    bool must_download,
    const DownloadInfo& info) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&DownloadController::StartAndroidDownload,
                 base::Unretained(this),
                 wc_getter, must_download, info));
}
