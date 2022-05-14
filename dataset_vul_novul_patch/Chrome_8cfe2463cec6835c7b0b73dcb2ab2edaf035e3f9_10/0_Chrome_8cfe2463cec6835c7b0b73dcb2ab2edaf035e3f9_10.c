 void KioskNextHomeInterfaceBrokerImpl::GetAppController(
     mojom::AppControllerRequest request) {
  AppControllerService::Get(context_)->BindRequest(std::move(request));
 }
