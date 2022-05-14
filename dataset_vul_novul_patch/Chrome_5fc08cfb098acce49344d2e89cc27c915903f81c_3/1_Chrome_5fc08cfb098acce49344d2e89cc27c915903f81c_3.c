void DownloadController::StartAndroidDownload(
    const content::ResourceRequestInfo::WebContentsGetter& wc_getter,
    bool must_download, const DownloadInfo& info) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  WebContents* web_contents = wc_getter.Run();
  if (!web_contents) {
    LOG(ERROR) << "Download failed on URL:" << info.url.spec();
    return;
  }
  AcquireFileAccessPermission(
      web_contents,
      base::Bind(&DownloadController::StartAndroidDownloadInternal,
                 base::Unretained(this), wc_getter, must_download, info));
}
