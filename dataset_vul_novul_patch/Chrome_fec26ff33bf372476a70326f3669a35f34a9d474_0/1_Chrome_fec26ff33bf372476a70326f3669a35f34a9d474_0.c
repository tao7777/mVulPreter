 void PageRequestSummary::UpdateOrAddToOrigins(
    const GURL& url,
     const content::mojom::CommonNetworkInfoPtr& network_info) {
  GURL origin = url.GetOrigin();
  if (!origin.is_valid())
     return;
 
   auto it = origins.find(origin);
  if (it == origins.end()) {
    OriginRequestSummary summary;
    summary.origin = origin;
    summary.first_occurrence = origins.size();
    it = origins.insert({origin, summary}).first;
  }

  it->second.always_access_network |= network_info->always_access_network;
  it->second.accessed_network |= network_info->network_accessed;
}
