 bool AddInitialUrlToPreconnectPrediction(const GURL& initial_url,
                                          PreconnectPrediction* prediction) {
  GURL initial_origin = initial_url.GetOrigin();
  static const int kMinSockets = 2;

  if (!prediction->requests.empty() &&
       prediction->requests.front().origin == initial_origin) {
     prediction->requests.front().num_sockets =
         std::max(prediction->requests.front().num_sockets, kMinSockets);
  } else if (initial_origin.is_valid() &&
             initial_origin.SchemeIsHTTPOrHTTPS()) {
    url::Origin origin = url::Origin::Create(initial_origin);
    prediction->requests.emplace(prediction->requests.begin(), initial_origin,
                                 kMinSockets,
                                 net::NetworkIsolationKey(origin, origin));
   }
 
   return !prediction->requests.empty();
}
