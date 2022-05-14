 void SecureProxyChecker::CheckIfSecureProxyIsAllowed(
     SecureProxyCheckerCallback fetcher_callback) {
   net::NetworkTrafficAnnotationTag traffic_annotation =
       net::DefineNetworkTrafficAnnotation(
           "data_reduction_proxy_secure_proxy_check", R"(
            semantics {
              sender: "Data Reduction Proxy"
              description:
                "Sends a request to the Data Reduction Proxy server. Proceeds "
                "with using a secure connection to the proxy only if the "
                "response is not blocked or modified by an intermediary."
              trigger:
                "A request can be sent whenever the browser is determining how "
                "to configure its connection to the data reduction proxy. This "
                "happens on startup and network changes."
              data: "A specific URL, not related to user data."
              destination: GOOGLE_OWNED_SERVICE
            }
            policy {
              cookies_allowed: NO
              setting:
                "Users can control Data Saver on Android via the 'Data Saver' "
                "setting. Data Saver is not available on iOS, and on desktop "
                "it is enabled by installing the Data Saver extension."
              policy_exception_justification: "Not implemented."
            })");
  auto resource_request = std::make_unique<network::ResourceRequest>();
  resource_request->url = params::GetSecureProxyCheckURL();
  resource_request->load_flags =
      net::LOAD_DISABLE_CACHE | net::LOAD_BYPASS_PROXY;
  resource_request->allow_credentials = false;
  url_loader_ = network::SimpleURLLoader::Create(std::move(resource_request),
                                                 traffic_annotation);

  static const int kMaxRetries = 5;
  url_loader_->SetRetryOptions(
      kMaxRetries, network::SimpleURLLoader::RETRY_ON_NETWORK_CHANGE |
                       network::SimpleURLLoader::RETRY_ON_5XX);
  url_loader_->SetOnRedirectCallback(base::BindRepeating(
      &SecureProxyChecker::OnURLLoaderRedirect, base::Unretained(this)));

  fetcher_callback_ = fetcher_callback;
  secure_proxy_check_start_time_ = base::Time::Now();

  url_loader_->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
      url_loader_factory_.get(),
      base::BindOnce(&SecureProxyChecker::OnURLLoadComplete,
                     base::Unretained(this)));
}
