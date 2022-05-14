 void PageRequestSummary::UpdateOrAddToOrigins(
     const content::mojom::ResourceLoadInfo& resource_load_info) {
  for (const auto& redirect_info : resource_load_info.redirect_info_chain) {
    UpdateOrAddToOrigins(url::Origin::Create(redirect_info->url),
                         redirect_info->network_info);
  }
  UpdateOrAddToOrigins(url::Origin::Create(resource_load_info.url),
                       resource_load_info.network_info);
 }
