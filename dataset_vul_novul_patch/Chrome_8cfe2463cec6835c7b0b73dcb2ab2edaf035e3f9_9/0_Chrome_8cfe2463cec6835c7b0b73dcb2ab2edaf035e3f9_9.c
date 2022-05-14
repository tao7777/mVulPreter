AppControllerImpl::~AppControllerImpl() {
AppControllerService::~AppControllerService() {
  app_service_proxy_->AppRegistryCache().RemoveObserver(this);
 }
