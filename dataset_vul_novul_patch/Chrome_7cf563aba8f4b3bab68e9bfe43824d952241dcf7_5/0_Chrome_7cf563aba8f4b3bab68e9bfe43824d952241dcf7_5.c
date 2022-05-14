void HostPortAllocatorSession::SendSessionRequest(const std::string& host,
                                                   int port) {
   GURL url("https://" + host + ":" + base::IntToString(port) +
            GetSessionRequestUrl() + "&sn=1");
  scoped_ptr<net::URLFetcher> url_fetcher(
      net::URLFetcher::Create(url, net::URLFetcher::GET, this));
   url_fetcher->SetRequestContext(url_context_);
  url_fetcher->AddExtraRequestHeader(
      "X-Talk-Google-Relay-Auth: " + relay_token());
  url_fetcher->AddExtraRequestHeader("X-Google-Relay-Auth: " + relay_token());
  url_fetcher->AddExtraRequestHeader("X-Stream-Type: chromoting");
  url_fetcher->Start();
   url_fetchers_.insert(url_fetcher.release());
 }
