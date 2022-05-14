void HostPortAllocatorSession::SendSessionRequest(const std::string& host,
                                                   int port) {
   GURL url("https://" + host + ":" + base::IntToString(port) +
            GetSessionRequestUrl() + "&sn=1");
  scoped_ptr<UrlFetcher> url_fetcher(new UrlFetcher(url, UrlFetcher::GET));
   url_fetcher->SetRequestContext(url_context_);
  url_fetcher->SetHeader("X-Talk-Google-Relay-Auth", relay_token());
  url_fetcher->SetHeader("X-Google-Relay-Auth", relay_token());
  url_fetcher->SetHeader("X-Stream-Type", "chromoting");
  url_fetcher->Start(base::Bind(&HostPortAllocatorSession::OnSessionRequestDone,
                                base::Unretained(this), url_fetcher.get()));
   url_fetchers_.insert(url_fetcher.release());
 }
