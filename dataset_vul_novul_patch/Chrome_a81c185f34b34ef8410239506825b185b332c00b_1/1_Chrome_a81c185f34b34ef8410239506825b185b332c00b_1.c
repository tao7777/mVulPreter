GaiaCookieManagerService::ExternalCcResultFetcher::CreateFetcher(
    const GURL& url) {
  std::unique_ptr<net::URLFetcher> fetcher =
      net::URLFetcher::Create(0, url, net::URLFetcher::GET, this);
   fetcher->SetRequestContext(helper_->request_context());
   fetcher->SetLoadFlags(net::LOAD_DO_NOT_SEND_COOKIES |
                         net::LOAD_DO_NOT_SAVE_COOKIES);
 
  fetcher->SetAutomaticallyRetryOnNetworkChanges(1);
  return fetcher;
}
