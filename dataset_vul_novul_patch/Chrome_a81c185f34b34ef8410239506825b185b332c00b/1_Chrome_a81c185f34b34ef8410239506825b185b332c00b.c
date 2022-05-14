void CaptivePortalDetector::DetectCaptivePortal(
    const GURL& url,
    const DetectionCallback& detection_callback) {
  DCHECK(CalledOnValidThread());
  DCHECK(!FetchingURL());
  DCHECK(detection_callback_.is_null());

  detection_callback_ = detection_callback;

   url_fetcher_ = net::URLFetcher::Create(0, url, net::URLFetcher::GET, this);
   url_fetcher_->SetAutomaticallyRetryOn5xx(false);
   url_fetcher_->SetRequestContext(request_context_.get());
 
  url_fetcher_->SetLoadFlags(
      net::LOAD_BYPASS_CACHE |
      net::LOAD_DO_NOT_SAVE_COOKIES |
      net::LOAD_DO_NOT_SEND_COOKIES |
      net::LOAD_DO_NOT_SEND_AUTH_DATA);
  url_fetcher_->Start();
}
