 void PageRequestSummary::UpdateOrAddToOrigins(
     const content::mojom::ResourceLoadInfo& resource_load_info) {
  for (const auto& redirect_info : resource_load_info.redirect_info_chain)
    UpdateOrAddToOrigins(redirect_info->url, redirect_info->network_info);
  UpdateOrAddToOrigins(resource_load_info.url, resource_load_info.network_info);
 }
