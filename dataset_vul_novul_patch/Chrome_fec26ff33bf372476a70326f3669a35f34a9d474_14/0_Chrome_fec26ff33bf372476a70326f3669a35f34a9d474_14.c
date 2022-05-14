bool ResourcePrefetchPredictor::PredictPreconnectOrigins(
    const GURL& url,
    PreconnectPrediction* prediction) const {
  DCHECK(!prediction || prediction->requests.empty());
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  if (initialization_state_ != INITIALIZED)
    return false;

  url::Origin url_origin = url::Origin::Create(url);
  url::Origin redirect_origin;
  bool has_any_prediction = GetRedirectEndpointsForPreconnect(
      url_origin, *host_redirect_data_, prediction);
  if (!GetRedirectOrigin(url_origin, *host_redirect_data_, &redirect_origin)) {
    return has_any_prediction;
  }

  OriginData data;
  if (!origin_data_->TryGetData(redirect_origin.host(), &data)) {
    return has_any_prediction;
  }

  if (prediction) {
    prediction->host = redirect_origin.host();
    prediction->is_redirected = (redirect_origin != url_origin);
  }

  net::NetworkIsolationKey network_isolation_key(redirect_origin,
                                                 redirect_origin);

  for (const OriginStat& origin : data.origins()) {
    float confidence = static_cast<float>(origin.number_of_hits()) /
                       (origin.number_of_hits() + origin.number_of_misses());
    if (confidence < kMinOriginConfidenceToTriggerPreresolve)
      continue;

     has_any_prediction = true;
     if (prediction) {
       if (confidence > kMinOriginConfidenceToTriggerPreconnect) {
        prediction->requests.emplace_back(
            url::Origin::Create(GURL(origin.origin())), 1,
            network_isolation_key);
       } else {
        prediction->requests.emplace_back(
            url::Origin::Create(GURL(origin.origin())), 0,
            network_isolation_key);
       }
     }
   }

  return has_any_prediction;
}
