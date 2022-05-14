bool ResourcePrefetchPredictor::GetRedirectEndpointsForPreconnect(
    const url::Origin& entry_origin,
    const RedirectDataMap& redirect_data,
    PreconnectPrediction* prediction) const {
  if (!base::FeatureList::IsEnabled(
          features::kLoadingPreconnectToRedirectTarget)) {
    return false;
  }
  DCHECK(!prediction || prediction->requests.empty());

  RedirectData data;
  if (!redirect_data.TryGetData(entry_origin.host(), &data))
    return false;

  const float kMinRedirectConfidenceToTriggerPrefetch = 0.1f;

  bool at_least_one_redirect_endpoint_added = false;
  for (const auto& redirect : data.redirect_endpoints()) {
    if (ComputeRedirectConfidence(redirect) <
        kMinRedirectConfidenceToTriggerPrefetch) {
      continue;
    }

    std::string redirect_scheme =
        redirect.url_scheme().empty() ? "https" : redirect.url_scheme();
    int redirect_port = redirect.has_url_port() ? redirect.url_port() : 443;

    const url::Origin redirect_origin = url::Origin::CreateFromNormalizedTuple(
        redirect_scheme, redirect.url(), redirect_port);

    if (redirect_origin == entry_origin) {
      continue;
    }

     if (prediction) {
       prediction->requests.emplace_back(
          redirect_origin, 1 /* num_scokets */,
           net::NetworkIsolationKey(redirect_origin, redirect_origin));
     }
     at_least_one_redirect_endpoint_added = true;
  }

  if (prediction && prediction->host.empty() &&
      at_least_one_redirect_endpoint_added) {
    prediction->host = entry_origin.host();
  }

  return at_least_one_redirect_endpoint_added;
}
