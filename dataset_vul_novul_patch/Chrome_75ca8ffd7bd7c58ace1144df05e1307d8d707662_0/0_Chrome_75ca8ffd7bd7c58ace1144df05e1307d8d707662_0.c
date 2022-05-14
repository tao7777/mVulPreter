int WebContentsImpl::DownloadImage(
    const GURL& url,
    bool is_favicon,
    uint32_t max_bitmap_size,
    bool bypass_cache,
    const WebContents::ImageDownloadCallback& callback) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  static int next_image_download_id = 0;
  const image_downloader::ImageDownloaderPtr& mojo_image_downloader =
      GetMainFrame()->GetMojoImageDownloader();
  const int download_id = ++next_image_download_id;
  if (!mojo_image_downloader) {
    image_downloader::DownloadResultPtr result =
        image_downloader::DownloadResult::New();
    result->http_status_code = 400;
     BrowserThread::PostTask(
         BrowserThread::UI, FROM_HERE,
        base::Bind(&WebContentsImpl::OnDidDownloadImage,
                   weak_factory_.GetWeakPtr(), callback, download_id, url,
                   base::Passed(&result)));
     return download_id;
   }
 
  image_downloader::DownloadRequestPtr req =
      image_downloader::DownloadRequest::New();

  req->url = mojo::String::From(url);
  req->is_favicon = is_favicon;
  req->max_bitmap_size = max_bitmap_size;
   req->bypass_cache = bypass_cache;
 
   mojo_image_downloader->DownloadImage(
      std::move(req), base::Bind(&WebContentsImpl::OnDidDownloadImage,
                                 weak_factory_.GetWeakPtr(), callback,
                                 download_id, url));
   return download_id;
 }
