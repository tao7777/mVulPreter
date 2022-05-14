AppControllerImpl::~AppControllerImpl() {
  if (apps::AppServiceProxy::Get(profile_))
    app_service_proxy_->AppRegistryCache().RemoveObserver(this);
 }
