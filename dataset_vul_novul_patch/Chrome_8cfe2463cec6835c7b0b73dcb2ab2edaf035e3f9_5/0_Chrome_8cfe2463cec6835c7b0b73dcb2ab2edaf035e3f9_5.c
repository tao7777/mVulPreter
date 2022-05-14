void AppControllerImpl::LaunchHomeUrl(const std::string& suffix,
void AppControllerService::LaunchHomeUrl(const std::string& suffix,
                                         LaunchHomeUrlCallback callback) {
   if (url_prefix_.empty()) {
     std::move(callback).Run(false, "No URL prefix.");
     return;
  }

  GURL url(url_prefix_ + suffix);
  if (!url.is_valid()) {
    std::move(callback).Run(false, "Invalid URL.");
    return;
  }

  arc::mojom::AppInstance* app_instance =
      arc::ArcServiceManager::Get()
          ? ARC_GET_INSTANCE_FOR_METHOD(
                arc::ArcServiceManager::Get()->arc_bridge_service()->app(),
                LaunchIntent)
          : nullptr;

  if (!app_instance) {
    std::move(callback).Run(false, "ARC bridge not available.");
    return;
  }

  app_instance->LaunchIntent(url.spec(), display::kDefaultDisplayId);
   std::move(callback).Run(true, base::nullopt);
 }
