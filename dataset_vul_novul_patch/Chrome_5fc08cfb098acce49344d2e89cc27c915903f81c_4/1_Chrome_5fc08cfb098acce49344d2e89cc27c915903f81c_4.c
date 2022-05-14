void DownloadController::StartAndroidDownloadInternal(
    const content::ResourceRequestInfo::WebContentsGetter& wc_getter,
    bool must_download, const DownloadInfo& info, bool allowed) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  if (!allowed)
    return;
  WebContents* web_contents = wc_getter.Run();
  if (!web_contents)
    return;
  base::string16 filename = net::GetSuggestedFilename(
      info.url, info.content_disposition,
      std::string(),  // referrer_charset
      std::string(),  // suggested_name
      info.original_mime_type,
      default_file_name_);
  ChromeDownloadDelegate::FromWebContents(web_contents)->RequestHTTPGetDownload(
      info.url.spec(), info.user_agent,
      info.content_disposition, info.original_mime_type,
      info.cookie, info.referer, filename,
      info.total_bytes, info.has_user_gesture,
      must_download);
}
