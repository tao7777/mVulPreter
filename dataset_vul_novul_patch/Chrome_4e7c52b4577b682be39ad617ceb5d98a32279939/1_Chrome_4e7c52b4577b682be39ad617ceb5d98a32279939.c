void WriteFromUrlOperation::Download(const base::Closure& continuation) {
  DCHECK_CURRENTLY_ON(BrowserThread::FILE);

  if (IsCancelled()) {
    return;
  }

  download_continuation_ = continuation;
 
   SetStage(image_writer_api::STAGE_DOWNLOAD);
 
  url_fetcher_ = net::URLFetcher::Create(url_, net::URLFetcher::GET, this);
 
   url_fetcher_->SetRequestContext(request_context_);
   url_fetcher_->SaveResponseToFileAtPath(
      image_path_, BrowserThread::GetTaskRunnerForThread(BrowserThread::FILE));

  AddCleanUpFunction(
      base::Bind(&WriteFromUrlOperation::DestroyUrlFetcher, this));

  url_fetcher_->Start();
}
