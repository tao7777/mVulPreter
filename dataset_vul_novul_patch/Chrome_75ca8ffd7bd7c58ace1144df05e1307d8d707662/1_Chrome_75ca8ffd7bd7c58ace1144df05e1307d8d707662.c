static void DidDownloadImage(const WebContents::ImageDownloadCallback& callback,
                             int id,
                             const GURL& image_url,
                             image_downloader::DownloadResultPtr result) {
  DCHECK(result);
  const std::vector<SkBitmap> images =
      result->images.To<std::vector<SkBitmap>>();
  const std::vector<gfx::Size> original_image_sizes =
      result->original_image_sizes.To<std::vector<gfx::Size>>();
  callback.Run(id, result->http_status_code, image_url, images,
               original_image_sizes);
}
