 void DataReductionProxyConfig::SecureProxyCheck(
     SecureProxyCheckerCallback fetcher_callback) {
  if (params::IsIncludedInHoldbackFieldTrial())
    return;

   secure_proxy_checker_->CheckIfSecureProxyIsAllowed(fetcher_callback);
 }
