 void DataReductionProxyConfig::SecureProxyCheck(
     SecureProxyCheckerCallback fetcher_callback) {
   secure_proxy_checker_->CheckIfSecureProxyIsAllowed(fetcher_callback);
 }
