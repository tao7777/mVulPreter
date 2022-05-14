void AppControllerImpl::GetApps(
void AppControllerService::GetApps(
     mojom::AppController::GetAppsCallback callback) {
   std::vector<chromeos::kiosk_next_home::mojom::AppPtr> app_list;
  app_service_proxy_->AppRegistryCache().ForEachApp(
      [this, &app_list](const apps::AppUpdate& update) {
        app_list.push_back(CreateAppPtr(update));
      });
   std::move(callback).Run(std::move(app_list));
 }
