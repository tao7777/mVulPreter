 bool AddInitialUrlToPreconnectPrediction(const GURL& initial_url,
                                          PreconnectPrediction* prediction) {
  url::Origin initial_origin = url::Origin::Create(initial_url);
  static const int kMinSockets = 2;

  if (!prediction->requests.empty() &&
       prediction->requests.front().origin == initial_origin) {
     prediction->requests.front().num_sockets =
         std::max(prediction->requests.front().num_sockets, kMinSockets);
  } else if (!initial_origin.opaque() &&
             (initial_origin.scheme() == url::kHttpScheme ||
              initial_origin.scheme() == url::kHttpsScheme)) {
    prediction->requests.emplace(
        prediction->requests.begin(), initial_origin, kMinSockets,
        net::NetworkIsolationKey(initial_origin, initial_origin));
   }
 
   return !prediction->requests.empty();
}
