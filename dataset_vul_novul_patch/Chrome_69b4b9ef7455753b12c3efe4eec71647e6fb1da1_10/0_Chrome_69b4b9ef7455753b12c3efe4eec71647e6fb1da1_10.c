 void WarmupURLFetcher::FetchWarmupURLNow(
     const DataReductionProxyServer& proxy_server) {
   DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(!params::IsIncludedInHoldbackFieldTrial());
 
   UMA_HISTOGRAM_EXACT_LINEAR("DataReductionProxy.WarmupURL.FetchInitiated", 1,
                              2);
  net::NetworkTrafficAnnotationTag traffic_annotation =
      net::DefineNetworkTrafficAnnotation("data_reduction_proxy_warmup", R"(
          semantics {
            sender: "Data Reduction Proxy"
            description:
              "Sends a request to the Data Reduction Proxy server to warm up "
              "the connection to the proxy."
            trigger:
              "A network change while the data reduction proxy is enabled will "
              "trigger this request."
            data: "A specific URL, not related to user data."
            destination: GOOGLE_OWNED_SERVICE
          }
          policy {
            cookies_allowed: NO
            setting:
              "Users can control Data Saver on Android via the 'Data Saver' "
              "setting. Data Saver is not available on iOS, and on desktop it "
              "is enabled by installing the Data Saver extension."
            policy_exception_justification: "Not implemented."
          })");

  GURL warmup_url_with_query_params;
  GetWarmupURLWithQueryParam(&warmup_url_with_query_params);

  url_loader_.reset();
  fetch_timeout_timer_.Stop();
  is_fetch_in_flight_ = true;

  auto resource_request = std::make_unique<network::ResourceRequest>();
  resource_request->url = warmup_url_with_query_params;
  resource_request->load_flags = net::LOAD_BYPASS_CACHE;

  resource_request->render_frame_id = MSG_ROUTING_CONTROL;

  url_loader_ = network::SimpleURLLoader::Create(std::move(resource_request),
                                                 traffic_annotation);
  static const int kMaxRetries = 5;
  url_loader_->SetRetryOptions(
      kMaxRetries, network::SimpleURLLoader::RETRY_ON_NETWORK_CHANGE);
  url_loader_->SetAllowHttpErrorResults(true);

  fetch_timeout_timer_.Start(FROM_HERE, GetFetchTimeout(), this,
                             &WarmupURLFetcher::OnFetchTimeout);

  url_loader_->SetOnResponseStartedCallback(base::BindOnce(
      &WarmupURLFetcher::OnURLLoadResponseStarted, base::Unretained(this)));
  url_loader_->SetOnRedirectCallback(base::BindRepeating(
      &WarmupURLFetcher::OnURLLoaderRedirect, base::Unretained(this)));

  url_loader_->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
      GetNetworkServiceURLLoaderFactory(proxy_server),
      base::BindOnce(&WarmupURLFetcher::OnURLLoadComplete,
                     base::Unretained(this)));
}
