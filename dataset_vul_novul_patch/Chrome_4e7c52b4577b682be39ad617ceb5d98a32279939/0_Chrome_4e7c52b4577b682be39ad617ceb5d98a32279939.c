void WriteFromUrlOperation::Download(const base::Closure& continuation) {
  DCHECK_CURRENTLY_ON(BrowserThread::FILE);

  if (IsCancelled()) {
    return;
  }

  download_continuation_ = continuation;
 
   SetStage(image_writer_api::STAGE_DOWNLOAD);
 
  // Create traffic annotation tag.
  net::NetworkTrafficAnnotationTag traffic_annotation =
      net::DefineNetworkTrafficAnnotation("cros_recovery_image_download", R"(
        semantics {
          sender: "Chrome OS Recovery Utility"
          description:
            "The Google Chrome OS recovery utility downloads the recovery "
            "image from Google Download Server."
          trigger:
            "User uses the Chrome OS Recovery Utility app/extension, selects "
            "a Chrome OS recovery image, and clicks the Create button to write "
            "the image to a USB or SD card."
          data:
            "URL of the image file to be downloaded. No other data or user "
            "identifier is sent."
          destination: GOOGLE_OWNED_SERVICE
        }
        policy {
          cookies_allowed: true
          cookies_store: "user"
          setting:
            "This feature cannot be disabled by settings, it can only be used "
            "by whitelisted apps/extension."
          policy_exception_justification:
            "Not implemented, considered not useful."
        })");

  url_fetcher_ = net::URLFetcher::Create(url_, net::URLFetcher::GET, this,
                                         traffic_annotation);
 
   url_fetcher_->SetRequestContext(request_context_);
   url_fetcher_->SaveResponseToFileAtPath(
      image_path_, BrowserThread::GetTaskRunnerForThread(BrowserThread::FILE));

  AddCleanUpFunction(
      base::Bind(&WriteFromUrlOperation::DestroyUrlFetcher, this));

  url_fetcher_->Start();
}
