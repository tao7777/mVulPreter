AppControllerImpl::AppControllerImpl(Profile* profile)
//// static
AppControllerService* AppControllerService::Get(
    content::BrowserContext* context) {
  return AppControllerServiceFactory::GetForBrowserContext(context);
}

AppControllerService::AppControllerService(Profile* profile)
     : profile_(profile),
       app_service_proxy_(apps::AppServiceProxy::Get(profile)),
       url_prefix_(base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
           chromeos::switches::kKioskNextHomeUrlPrefix)) {
  DCHECK(profile);
   app_service_proxy_->AppRegistryCache().AddObserver(this);
 
  content::URLDataSource::Add(profile,
                              std::make_unique<apps::AppIconSource>(profile));
 }
