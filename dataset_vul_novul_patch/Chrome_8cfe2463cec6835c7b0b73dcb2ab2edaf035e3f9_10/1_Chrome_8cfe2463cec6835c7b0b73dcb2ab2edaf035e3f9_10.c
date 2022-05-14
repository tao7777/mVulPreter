 void KioskNextHomeInterfaceBrokerImpl::GetAppController(
     mojom::AppControllerRequest request) {
  app_controller_->BindRequest(std::move(request));
 }
