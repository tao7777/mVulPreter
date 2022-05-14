void AppControllerImpl::LaunchApp(const std::string& app_id) {
   app_service_proxy_->Launch(app_id, ui::EventFlags::EF_NONE,
                              apps::mojom::LaunchSource::kFromAppListGrid,
                              display::kDefaultDisplayId);
 }
